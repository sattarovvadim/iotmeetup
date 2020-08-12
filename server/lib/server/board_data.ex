defmodule Server.BoardData do
  defstruct simple_led: 0,
            sock_220v: 0,
            servo: 0,
            is_light: 0,
            potent: 0,
            range: 0,
            is_motion: 0,
            is_barrier: 0,
            temperature: 0

  def init_storage() do
    :ets.new(__MODULE__, [:set, :public, :named_table])
    store(%__MODULE__{})
  end

  def store(%__MODULE__{} = value) do
    value
    |> Map.keys()
    |> Enum.each(fn key ->
      :ets.insert(__MODULE__, {key, Map.get(value, key)})
    end)
  end

  def get() do
    :ets.tab2list(__MODULE__) |> Map.new() |> Map.from_struct()
  end

  def parse_json(string) do
    case Jason.decode(string, keys: :atoms!) do
      {:ok, result} ->
        struct(__MODULE__, result)

      _ ->
        nil
    end
  end
end
