import { useEffect, useRef } from "react";

function RectangleCanvas() {
    const canvasRef = useRef<HTMLCanvasElement>(null);
  
    useEffect(() => {
      const canvas = canvasRef.current;
      if (!canvas) return;
      const ctx = canvas.getContext("2d");
      if (!ctx) return;
  
      ctx.fillStyle = "rgba(255, 0, 0, 0.5)";
      ctx.fillRect(50, 50, 200, 100); // x, y, width, height
    }, []);
  
    return <canvas ref={canvasRef} width={400} height={300} className="border" />;
  }

export default RectangleCanvas;