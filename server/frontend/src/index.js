import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import * as serviceWorker from './serviceWorker';

import ChannelProvider, { createChannel } from 'phoenix-react'

//  Refers the initial state you want
const state = {
  simple_led: 0,
  sock_220v: 0,
  servo: 0,
  is_light: 0,
  potent: 0,
  range: 0,
  is_motion: 0,
  is_barrier: 0,
  temperature: 0
}

const iotChannel = createChannel('iotmeetup:data', (channel, { mutate }) => {
  for (let key in state) {
    if (state.hasOwnProperty(key)) {
      channel.on(key, (body) => {
        mutate(state => { state[key] = body[key]; return state; })
      })
    }
  }

  channel.on('data', ({ body }) => {
    console.log(body);
    mutate(state => body)
  })

  channel.join()
    .receive("ok", resp => { console.log("Joined successfully", resp); mutate(state => resp); })
    .receive("error", resp => { console.log("Unable to join", resp) })

  return channel;
})


//  Refering to the `opts` parameter for Pheonix.Socket instanciation
const params = {
  params: {
    token: localStorage.token,
  },
}

ReactDOM.render(
  <React.StrictMode>
    <ChannelProvider
      url="ws://localhost:4000/socket"
      channels={[iotChannel]}
      state={state}
      params={params}
    >
      <App values={state} />
    </ChannelProvider>
  </React.StrictMode>,
  document.getElementById('root')
);

serviceWorker.unregister();
