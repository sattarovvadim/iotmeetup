defmodule Server do
  @moduledoc """
  Server keeps the contexts that define your domain
  and business logic.

  Contexts are also responsible for managing your data, regardless
  if it comes from the database, an external API or others.
  """

  # publish a message on the broker
  # Tortoise.publish("my_client_id", "foo/bar", "Hello from the World of Tomorrow !", qos: 0)
end
