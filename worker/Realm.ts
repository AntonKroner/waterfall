import { gracely } from "gracely"
import { DurableObject } from "cloudflare:workers"
import type { Environment } from "./Environment"

export class Realm extends DurableObject<Environment> {
	#messages?: string[]
	get messages(): Promise<string[]> {
		return Promise.resolve().then(
			async () => this.#messages ?? (await this.state.storage.get<string[]>("messages")) ?? []
		)
	}
	set messages(message: string) {
		this.messages.then(m => {
			m.length > 15 && m.shift()
			m.push(message)
			this.state.storage.put("messages", m)
		})
	}
	constructor(private readonly state: DurableObjectState, readonly environment: Environment) {
		super(state, environment)
	}
	async exportedMethod(thingy: string): Promise<string> {
		return thingy
	}
	async webSocketMessage(socket: WebSocket, message: string | ArrayBuffer): Promise<void> {
		const sockets = this.state.getWebSockets()
		const id = socket.deserializeAttachment()
		console.log("message: ", message)
		if (message == "logout") {
			socket.close(1000, "user requested logout")
			sockets.forEach(s => s != socket && s.send(`${id}: logged out`))
		} else {
			const post = `${id}: ${message}`
			this.messages = post
			sockets.forEach(s => s.send(post))
		}
	}
	async webSocketClose(socket: WebSocket, code: number, reason: string, clean: boolean): Promise<void> {
		console.log("socket closed with: ", code, reason, clean)
		socket.close()
	}
	async webSocketError(socket: WebSocket, error: unknown): Promise<void> {
		console.log("socket error: ", error)
		socket.close()
	}
	async fetch(request: Request): Promise<Response> {
		let result: Response
		let user: string | null
		if (request.headers.get("Upgrade") != "websocket")
			result = Response.json(gracely.client.malformedHeader("upgrade", "Expected: websocket"), { status: 400 })
		else if (!(user = request.headers.get("user")))
			result = Response.json(gracely.client.malformedHeader("user", "Expected username"), { status: 400 })
		else {
			const [client, server] = Object.values(new WebSocketPair())
			this.state.acceptWebSocket(server)
			server.serializeAttachment(user)
			;(await this.messages).map(m => server.send(m))
			this.state.getWebSockets().map(s => s.send(`${user}: logged in`))
			result = new Response(null, { status: 101, webSocket: client })
		}
		return result
	}
}
