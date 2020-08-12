import { createChannel } from 'phoenix-react'

//  Messages mutators. These are helpers that will mutate the state withing a payload. Even though these are not required, they are recommended.
const messageMutators = {
    add: (state, message) => ({
        ...state,
        messages: [
            ...state.messages,
            message
        ],
    }),
    set: (state, messages) => ({
        ...state,
        messages: messages,
    }),
}

//  The message channel, the mutate function performs a state mutation of the provider which will re-render the consumer components.
const iotChannel = createChannel('iotmeetup:data', (channel, { mutate }) => {

    //  You can add handlers for different events. 
    //This one with add a message to the state when the `new_msg` event is broadcasted by the Phoenix server
    channel.on('data', ({ body }) => {
        console.log(body);
        //mutate(state => messageMutators.add(state, body.message))
    })

    channel.join()
        .receive("ok", resp => { console.log("Joined successfully", resp) })
        .receive("error", resp => { console.log("Unable to join", resp) })

    return channel;
})

export default [
    iotChannel
]