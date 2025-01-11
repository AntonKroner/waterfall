import { gracely } from "gracely"
import { http } from "cloudly-http"
import { storage } from "cloudly-storage"
import { Environment } from "./Environment"
import { Message } from "./Message"

export default {
	async fetch(request: Request, environment: Environment): Promise<Response> {
		let result: Response
		console.log("method: ", request.method)
		console.log("headers: ", [...request.headers.entries()])
		const namespace = storage.DurableObject.Namespace.open(environment.realmNamespace)
		let authentication: http.Authorization.Basic | undefined
		if (!(authentication = http.Authorization.Basic.parse(request.headers.get("authorization") ?? undefined)))
			result = Response.json(gracely.client.unauthorized("basic"), { status: 401 })
		else if (authentication.user == "problem")
			result = Response.json(gracely.client.unauthorized("basic"), { status: 401 })
		else if (authentication.user == "forbidden")
			result = Response.json(gracely.client.forbidden("used the forbidden user"), { status: 403 })
		else if (authentication.user == "not found")
			result = Response.json(gracely.client.notFound("not found test"), { status: 404 })
		else if (!namespace)
			result = Response.json(gracely.server.misconfigured("realmNamespace", "Namespace missing"), { status: 503 })
		else if (request.headers.get("Upgrade") != "websocket")
			result = Response.json(gracely.client.malformedHeader("upgrade", "Expected: websocket"), { status: 400 })
		else {
			const client = namespace.open("socket")
			const socket = await client.socket("/socket", { user: authentication.user })
			result = !socket
				? Response.json(gracely.server.misconfigured("realmNamespace", "Socket missing"), { status: 503 })
				: new Response(null, { status: 101, webSocket: socket })
		}
		return result
	},
}

interface Player {
	id: number
	name: string
	position: [number, number, number]
}

export class Realm implements DurableObject {
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
	constructor(private readonly state: DurableObjectState, readonly environment: Environment) {}

	async webSocketMessage(socket: WebSocket, payload: string | ArrayBuffer): Promise<void> {
		const sockets = this.state.getWebSockets()
		const player: Player = socket.deserializeAttachment()
		if (typeof payload != "string") {
			console.log("message is arraybuffer: ", payload)
			const message = Message.parse(payload)
			console.log("decoded: ", message)
			switch (message?.type) {
				case Message.Type.chat:
					const post = `${player.name}: ${message.message}`
					this.messages = post
					sockets.forEach(s => s.send(post))
					break
			}
		} else {
			console.log("message is string: ", payload)
			if (payload == "logout") {
				socket.close(1000, "user requested logout")
				sockets.forEach(s => s != socket && s.send(`${player.name}: logged out`))
			} else if (payload == "moved") {
				player.position[0] += 0.1
				sockets.forEach(s => s.send(`player ${player.id}: ${player.position}`))
				socket.serializeAttachment(player)
			} else {
				const post = `${player.name}: ${payload}`
				this.messages = post
				sockets.forEach(s => s.send(post))
			}
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
			;(await this.messages).map(m => server.send(m))
			const sockets = this.state.getWebSockets()

			const newPlayer: Player = { id: sockets.length - 1, name: user, position: [0, 0, 0] }
			server.serializeAttachment(newPlayer)
			console.log(sockets.map(s => s.deserializeAttachment()))
			sockets.map(s => {
				const oldPlayer: Player = s.deserializeAttachment()
				if (oldPlayer.id != newPlayer.id) {
					const message = `player ${oldPlayer.id}: logged in`
					server.send(message)
				}
				s.send(`player ${newPlayer.id}: logged in`)
			})
			result = new Response(null, { status: 101, webSocket: client })
		}
		return result
	}
}
