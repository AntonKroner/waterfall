export interface Environment
	extends Record<string, undefined | string | KVNamespace | DurableObjectNamespace | Fetcher> {
	environment?: string
	realmNamespace?: DurableObjectNamespace
}
