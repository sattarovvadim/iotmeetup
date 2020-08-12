defmodule Server.MqttHandler do
  @moduledoc """
    Обработчик событий протокола MQTT, реализует колбеки из Tortoise.Handler
  """

  require Logger

  alias Server.BoardData
  alias ServerWeb.IotMeetupChannel

  use Tortoise.Handler

  ## API

  def request_data_from_board() do
    Logger.info("Initial board data requested")

    Tortoise.publish(
      Application.fetch_env!(:server, :client_id),
      "iotmeetup/espdevice/command/request",
      <<0>>
    )
  end

  def command_to_board(node, value) do
    Tortoise.publish(
      Application.fetch_env!(:server, :client_id),
      "iotmeetup/espdevice/command/#{Atom.to_string(node)}",
      prepare_payload(value)
    )
  end

  ## Callbacks

  def init(_opts) do
    {:ok, %BoardData{}}
  end

  def connection(:up, state) do
    Logger.info("Connection has been established")
    request_data_from_board()

    {:ok, state}
  end

  def subscription(:up, topic, state) do
    Logger.info("Subscribed to #{topic}")
    {:ok, state}
  end

  ## Обработка входящих сообщений от платы

  def handle_message(["iotmeetup", "espdevice", node], <<data>>, state) do
    Logger.info("topic: #{node} #{inspect(data)}")

    key = String.to_existing_atom(node)

    new_state = Map.put(state, key, data)
    BoardData.store(new_state)
    IotMeetupChannel.broadcast_data(node, Map.put(%{}, key, data))

    {:ok, new_state}
  end

  def handle_message(["iotmeetup", "espdevice", "data"], publish, state) do
    Logger.info("topic: data #{inspect(publish)}")

    {:ok, handle_incoming_data(publish, state)}
  end

  def handle_message(["iotmeetup", "espdevice", "handshake"], publish, state) do
    {:ok, handle_incoming_data(publish, state)}
  end

  def handle_message(topic, publish, state) do
    Logger.info("Unknown topic #{Enum.join(topic, "/")} #{inspect(publish)}")
    {:ok, state}
  end

  ## Private
  defp handle_incoming_data(buffer, state) do
    case BoardData.parse_json(buffer) do
      nil ->
        state

      new_data ->
        BoardData.store(new_data)
        IotMeetupChannel.broadcast_data("data", Map.from_struct(new_data))
        new_data
    end
  end

  defp prepare_payload(true), do: <<1>>
  defp prepare_payload(false), do: <<0>>
  defp prepare_payload(value), do: <<value::8>>
end
