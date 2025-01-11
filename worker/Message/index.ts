export type Message = Message.Login | Message.Chat | Message.Move
export namespace Message {
	export enum Type {
		login = 0,
		chat,
		move,
	}
	export interface Login {
		type: Type.login
		name: string
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
			case Message.Type.chat:
				const message = new Uint8Array(buffer)
				result = {
					type: Message.Type.chat,
					message: new TextDecoder().decode(message.subarray(4, message.indexOf(0, 4))),
				}
				break
		}
		return result
	}
}
