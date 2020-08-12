defmodule Server.Application do
  # See https://hexdocs.pm/elixir/Application.html
  # for more information on OTP Applications
  @moduledoc false

  use Application

  def start(_type, _args) do
    children = [
      # Start the Ecto repository
      Server.Repo,
      # Start the Telemetry supervisor
      ServerWeb.Telemetry,
      # Start the PubSub system
      {Phoenix.PubSub, name: Server.PubSub},
      # Start the Endpoint (http/https)
      ServerWeb.Endpoint
    ]

    # See https://hexdocs.pm/elixir/Supervisor.html
    # for other strategies and supported options
    opts = [strategy: :one_for_one, name: Server.Supervisor]
    Supervisor.start_link(children, opts)

    Server.BoardData.init_storage()

    # connect to the server and subscribe to foo/bar with QoS 0
    Tortoise.Supervisor.start_child(
      client_id: Application.fetch_env!(:server, :client_id),
      handler: {Server.MqttHandler, []},
      server:
        {Tortoise.Transport.Tcp,
         host: Application.fetch_env!(:server, :host),
         port: Application.fetch_env!(:server, :port)},
      subscriptions: [{"iotmeetup/espdevice/#", 0}]
    )
  end

  # Tell Phoenix to update the endpoint configuration
  # whenever the application is updated.
  def config_change(changed, _new, removed) do
    ServerWeb.Endpoint.config_change(changed, removed)
    :ok
  end
end
