import { DurableObject } from "cloudflare:workers"
import { Client } from "./Client"

export class Namespace<T extends DurableObject | undefined = undefined> {
	#objects: Record<string, Client<T> | undefined> = {}
	private constructor(private readonly backend: DurableObjectNamespace<T>, readonly partitions = "") {}

	open(name?: string, id?: string, options?: DurableObjectNamespaceNewUniqueIdOptions): Client<T> {
		return typeof name == "string"
			? (this.#objects[name] ??= new Client<T>(this.backend.get(this.backend.idFromName(this.partitions + name))))
			: typeof id == "string"
			? new Client<T>(this.backend.get(this.backend.idFromString(id)))
			: new Client<T>(this.backend.get(this.backend.newUniqueId(options)))
	}
	partition(...partition: string[]): Namespace<T> {
		return new Namespace<T>(this.backend, this.partitions + partition.join("/") + "/")
	}
	static open<T extends DurableObject | undefined = undefined>(
		backend: DurableObjectNamespace<T> | undefined,
		partition?: string
	): Namespace<T> | undefined {
		return backend && new Namespace<T>(backend, partition)
	}
}
