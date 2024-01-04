import { useEffect, useState } from "react";
import "./App.css";
const gateway = `ws://192.168.86.111/ws`;

function App() {
  const [sensorData, setSensorData] = useState<{
    distance: string;
  }>();

  const onOpen = (ws: WebSocket) => {
    ws.send("getReadings");
  };
  const onClose = () => {
    console.log("Connection closed");
  };
  const onMessage = (evt: MessageEvent) => {
    const myObj = JSON.parse(evt.data);
    setSensorData(myObj);
  };

  useEffect(() => {
    console.log("Trying to open a WebSocket connection…");
    const websocket = new WebSocket(gateway);
    websocket.onopen = () => onOpen(websocket);
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
  }, []);

  return (
    <>
      <h1>Espresso Machine Water Level</h1>
      <p>{sensorData?.distance || 0}%</p>
    </>
  );
}

export default App;
