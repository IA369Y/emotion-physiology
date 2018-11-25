import java.io.BufferedReader;
import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class ThinkGearClientSample implements Runnable {

  private static final String ENABLE_RAW_OUTPUT_JSON = "{\"enableRawOutput\":false,\"format\":\"Json\"}";
  private static final String AUTH_JSON = String.format("{\"appName\":\"%s\",\"appKey\":\"%s\"}", "ThinkGearClient",
      "8997C3C6D4DD22541F0558504F7952370411F402");

  private Socket thinkGearSocket;
  private OutputStream os;
  private InputStream is;
  private BufferedReader br;
  private Thread thread;

  public static void main(String[] args) throws Exception {
    ThinkGearClientSample tgc = new ThinkGearClientSample();
    tgc.init();
  }

  @Override
  public void run() {
    if (thinkGearSocket.isConnected()) {
      String thinkGearData;
      try {
        while ((thinkGearData = br.readLine()) != null) {
          System.out.println(thinkGearData);
        }
      } catch (IOException e) {
        e.printStackTrace();
      }
    }

  }

  private void init() throws Exception {
    thinkGearSocket = new Socket("127.0.0.1", 13854);
    is = thinkGearSocket.getInputStream();
    os = thinkGearSocket.getOutputStream();
    br = new BufferedReader(new InputStreamReader(is));
    sendMessage(ENABLE_RAW_OUTPUT_JSON);
    thread = new Thread(this);
    thread.start();
  }

  private void sendMessage(final String msg) {
    PrintWriter out = new PrintWriter(os, true);
    out.println(msg);
  }

  public void stop(){

    if(thread.isAlive()){
      thread.interrupt();
    }
    finalizeStreams();
  }

  private void finalizeStreams() {
    closeStream(os);
    closeStream(is);
    closeStream(br);
    closeStream(thinkGearSocket);
  }

  private void closeStream(final Closeable closeable) {
    try {
      closeable.close();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
