defmodule ServerWeb.IotMeetupChannel do
  use Phoenix.Channel

  def join("iotmeetup:data", _message, socket) do
    {:ok, Server.BoardData.get(), socket}
  end

  def join("iotmeetup:" <> _sensor_name, _params, _socket) do
    {:error, %{reason: "unauthorized"}}
  end

  def handle_in("command", %{"command" => command, "value" => value}, socket) do
    Server.MqttHandler.command_to_board(String.to_existing_atom(command), value)
    {:noreply, socket}
  end

  def broadcast_data(sensor, value) do
    ServerWeb.Endpoint.broadcast!("iotmeetup:data", sensor, value)
  end
end
