export type Message = Message.Login | Message.Chat | Message.Move
export namespace Message {
	export enum Type {
		login = 0,
		chat,
		move,
	}
	export interface Login {
		type: Type.login
		id: number
		name: string
		position: [number, number, number]
	}
	export namespace Login {
		export function serialize(player: Omit<Login, "type">): ArrayBuffer {
			const result: ArrayBuffer = new ArrayBuffer(4 * 5 + 256)
			const view = new DataView(result)
			view.setUint32(0, Type.chat, true)
			new TextEncoder().encodeInto(player.name, new Uint8Array(result).subarray(4))
			view.setFloat32(260, player.position[0], true)
			view.setFloat32(264, player.position[1], true)
			view.setFloat32(268, player.position[2], true)
			return result
		}
	}
	export interface Chat {
		type: Type.chat
		message: string
	}
	export interface Move {
		type: Type.move
		direction: number
	}
	export function parse(buffer: ArrayBuffer): Message | undefined {
		let result: Message | undefined = undefined
		const view = new DataView(buffer)
		switch (view.getUint32(0, true)) {
			case Type.chat:
				const message = new Uint8Array(buffer)
				result = {
					type: Type.chat,
					message: new TextDecoder().decode(message.subarray(4, message.indexOf(0, 4))),
				}
				break
			case Type.login:
				const name = new Uint8Array(buffer)
				result = {
					type: Type.login,
					id: 0,
					name: new TextDecoder().decode(name.subarray(4, name.indexOf(0, 4))),
					position: [view.getFloat32(260, true), view.getFloat32(264, true), view.getFloat32(268, true)],
				}
				break
		}
		return result
	}
	export function serialize(message: Message.Chat): ArrayBuffer {
		let result: ArrayBuffer
		switch (message.type) {
			case Type.chat:
				result = new ArrayBuffer(4 + message.message.length)
				const view = new DataView(result)
				view.setUint32(0, Type.chat, true)
				new TextEncoder().encodeInto(message.message, new Uint8Array(result).subarray(4))
				break
		}
		return result
	}
}
