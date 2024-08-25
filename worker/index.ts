import { gracely } from "gracely"
import { storage } from "cloudly-storage"
import { Environment } from "./Environment"

export default {
	async fetch(request: Request, environment: Environment): Promise<Response> {
		let result: Response
		const namespace = storage.DurableObject.Namespace.open(environment.realmNamespace)
		if (!namespace)
			result = Response.json(gracely.server.misconfigured("realmNamespace", "Namespace missing"), { status: 503 })
		else if (request.headers.get("Upgrade") != "websocket")
			result = Response.json(gracely.client.malformedHeader("upgrade", "Expected: websocket"), { status: 400 })
		else {
			const client = namespace.open("socket")
			const socket = await client.socket("/socket")
			result = !socket
				? Response.json(gracely.server.misconfigured("realmNamespace", "Socket missing"), { status: 503 })
				: new Response(null, { status: 101, webSocket: socket })
		}
		return result
	},
}
export class Realm implements DurableObject {
	constructor(private readonly state: DurableObjectState, readonly environment: Environment) {}

	async webSocketMessage(socket: WebSocket, message: string | ArrayBuffer): Promise<void> {
		const sockets = this.state.getWebSockets()
		const id = socket.deserializeAttachment()
		console.log("message: ", message)
		sockets.map(s => s.send(`${id}: ${message}`))
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
		if (request.headers.get("Upgrade") != "websocket")
			result = Response.json(gracely.client.malformedHeader("upgrade", "Expected: websocket"), { status: 400 })
		else {
			const [client, server] = Object.values(new WebSocketPair())
			this.state.acceptWebSocket(server)
			server.serializeAttachment(Math.floor(Math.random() * 128).toString(16))
			result = new Response(null, { status: 101, webSocket: client })
		}
		return result
	}
}
