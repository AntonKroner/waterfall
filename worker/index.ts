import { gracely } from "gracely"
import { http } from "cloudly-http"
import { common } from "./common"
import { Environment } from "./Environment"

export default {
	async fetch(request: Request, environment: Environment): Promise<Response> {
		let result: Response
		console.log("method: ", request.method)
		console.log("headers: ", [...request.headers.entries()])
		const namespace = common.DurableObject.Namespace.open(environment.realmNamespace)
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
			client.methods.exportedMethod
			const socket = await client.socket("/socket", { user: authentication.user })
			result = !socket
				? Response.json(gracely.server.misconfigured("realmNamespace", "Socket missing"), { status: 503 })
				: new Response(null, { status: 101, webSocket: socket })
		}
		return result
	},
}
export { Realm } from "./Realm"
