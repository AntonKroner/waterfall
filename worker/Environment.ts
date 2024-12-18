import type { Realm } from "./Realm"

export interface Environment
	extends Record<string, undefined | string | KVNamespace | DurableObjectNamespace | Fetcher> {
	environment?: string
	realmNamespace?: DurableObjectNamespace<Realm>
}
