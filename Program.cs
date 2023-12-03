using System;
using System.Net.WebSockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using NAudio.Wave;
using System.Collections.Generic;
using Newtonsoft.Json;
using System.IO;

class Program
{
  [DllImport("kernel32.dll")]
  static extern IntPtr GetConsoleWindow();

  [DllImport("user32.dll")]
  static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

  const int SW_HIDE = 0;
  private static ClientWebSocket? client; // Declarar la variable a nivel de clase
  private static Dictionary<string, AudioFileReader> sonidos;
  private static WaveOutEvent outputDevice;
  private static Timer timerSound;
  private static Timer timer;
  private static Timer timerClose;
  static Program()
  {
    sonidos = new Dictionary<string, AudioFileReader>();
    sonidos.Add("open", new AudioFileReader("sonidos/open.mp3"));
    sonidos.Add("close", new AudioFileReader("sonidos/close.mp3"));
    sonidos.Add("on", new AudioFileReader("sonidos/on.mp3"));
    sonidos.Add("off", new AudioFileReader("sonidos/off.mp3"));
    sonidos.Add("hurry", new AudioFileReader("sonidos/hurry.mp3"));
    sonidos.Add("tuturu", new AudioFileReader("sonidos/tuturu.mp3"));
    outputDevice = new WaveOutEvent();

    timerSound = new Timer(TimerSoundCallback, null, Timeout.Infinite, Timeout.Infinite);
    timerClose = new Timer(TimerCloseCallback, null, Timeout.Infinite, Timeout.Infinite);
    timer = new Timer(TimerCallback, null, Timeout.Infinite, Timeout.Infinite);
  }
  static async Task Main(string[] args)
  {
    var handle = GetConsoleWindow();
    ShowWindow(handle, SW_HIDE);

    ServerSettings serverSettings = LoadServerSettings("config.json");

    while (true)
    {
      using (client = new ClientWebSocket())
      {
        client.Options.SetRequestHeader("Sec-WebSocket-Extensions", "permessage-deflate; client_max_window_bits");
        client.Options.KeepAliveInterval = TimeSpan.FromSeconds(5);
        Console.WriteLine("...");

        try
        {
          Uri serverUri = new Uri(serverSettings.ServerUrl);
          await client.ConnectAsync(serverUri, CancellationToken.None);
          ReproducirSonido(outputDevice, sonidos["tuturu"]);

          // Temporizador para el primer mensaje

          await Task.Delay(2000);

          while (client.State == WebSocketState.Open)
          {
            byte[] buffer = new byte[1024];
            WebSocketReceiveResult result = await client.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

            if (result.MessageType == WebSocketMessageType.Text)
            {
              string message = Encoding.UTF8.GetString(buffer, 0, result.Count);
              Console.WriteLine(message);

              if (message == "derecha_off")
              {
                ReproducirSonido(outputDevice, sonidos["open"]);
              }
              else if (message == "derecha_on")
              {
                ReproducirSonido(outputDevice, sonidos["close"]);
              }
              else if (message == "izquierda_off")
              {
                ReproducirSonido(outputDevice, sonidos["open"]);
              }
              else if (message == "izquierda_on")
              {
                ReproducirSonido(outputDevice, sonidos["close"]);
              }

              // Cancelar el temporizador existente
              timer.Change(Timeout.Infinite, Timeout.Infinite);
              timerSound.Change(Timeout.Infinite, Timeout.Infinite);
              timerClose.Change(Timeout.Infinite, Timeout.Infinite);

              // Iniciar un nuevo temporizador de 5 segundos
              timer.Change(5000, Timeout.Infinite);

            }
          }
        }
        catch (WebSocketException ex)
        {
          Console.WriteLine("Se perdió la conexión con el servidor.");
          // ReproducirSonido(outputDevice, sonidos["hurry"]);
          timerSound.Change(0, 5000);


          if (ex.WebSocketErrorCode == WebSocketError.ConnectionClosedPrematurely)
          {
            Console.WriteLine("Intentando reconectar...");
          }
          else
          {
            Console.WriteLine("Error de WebSocket: " + ex.Message);
          }

        }
        catch (Exception ex)
        {
          ReproducirSonido(outputDevice, sonidos["close"]);
          Console.WriteLine("Error inesperado: " + ex.Message);
        }
      }
    }
  }


  private static async void TimerCallback(object? state)
  {
    // Console.WriteLine("Han pasado 5 segundos sin recibir mensajes. Realiza la acción que deseas aquí.");
    // ReproducirSonido(outputDevice, sonidos["off"]);
    // Console.WriteLine("TimerCallback");

    if (client != null && client.State == WebSocketState.Open)
    {
      try
      {
        byte[] messageBytes = Encoding.UTF8.GetBytes("ping");
        await client.SendAsync(new ArraySegment<byte>(messageBytes), WebSocketMessageType.Text, true, CancellationToken.None);
      }
      catch (WebSocketException ex)
      {
        Console.WriteLine("Error al enviar el mensaje: " + ex.Message);
      }
    }
    timerClose.Change(5000, Timeout.Infinite);

  }
  private static void TimerCloseCallback(object? state)
  {
    // Console.WriteLine("TimerCloseCallback");

    // Console.WriteLine("Han pasado 5 segundos sin recibir mensajes. Realiza la acción que deseas aquí.");
    // ReproducirSonido(outputDevice, sonidos["off"]);
    timerSound.Change(0, 5000);
  }
  private static void TimerSoundCallback(object? state)
  {
    // Console.WriteLine("TimerSoundCallback");

    // Console.WriteLine("Han pasado 5 segundos sin recibir mensajes. Realiza la acción que deseas aquí.");
    ReproducirSonido(outputDevice, sonidos["hurry"]);

  }

  static ServerSettings LoadServerSettings(string configFile)
  {
    try
    {
      string json = File.ReadAllText(configFile);
      ServerSettings? serverSettings = JsonConvert.DeserializeObject<ServerSettings>(json);

      if (serverSettings != null)
      {
        return serverSettings;
      }
      else
      {
        Console.WriteLine("La deserialización de la configuración ha fallado. Se usará un valor predeterminado.");
        // Puedes manejar la situación de error de la manera que desees.
      }
    }
    catch (Exception ex)
    {
      Console.WriteLine("Error al cargar la configuración: " + ex.Message);
      // Puedes manejar el error de la manera que desees, como proporcionar un valor predeterminado.
    }

    // Si ocurre algún error o la deserialización falla, se devuelve un valor predeterminado.
    return new ServerSettings { ServerUrl = "ws://192.168.1.82:81/" };
  }

  static void ReproducirSonido(WaveOutEvent outputDevice, AudioFileReader audioFile)
  {
    try
    {
      outputDevice.Stop(); // Detener la reproducción anterior si la hay
      audioFile.Position = 0; // Reiniciar el flujo de audio al principio
      outputDevice.Init(audioFile);

      // Declarar la variable `tcs`
      // var tcs = new TaskCompletionSource<object>();

      // // Establecer el manejador de eventos
      // EventHandler<StoppedEventArgs>? handler = null;
      // handler = (sender, e) =>
      // {
      //   outputDevice.PlaybackStopped -= handler;
      //   tcs.SetResult(new object());
      // };
      // outputDevice.PlaybackStopped += handler;

      // Reproducir el sonido de manera asincrónica
      outputDevice.Play();

      // Esperar hasta que la reproducción termine
      // await tcs.Task;
    }
    catch (Exception ex)
    {
      Console.WriteLine("Error al reproducir el sonido: " + ex.Message);
    }
  }
}

public class ServerSettings
{
  public string ServerUrl { get; set; }

  public ServerSettings()
  {
    ServerUrl = "ws://192.168.1.82:81/"; // Valor predeterminado
  }
}