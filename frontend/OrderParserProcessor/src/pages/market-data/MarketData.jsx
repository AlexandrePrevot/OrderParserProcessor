import { useRef, useEffect } from "react";
import {
    Chart as ChartJS,
    LineElement,
    PointElement,
    LinearScale,
    TimeScale,
    Tooltip,
    Filler,
    ScatterController,
} from "chart.js";
import "chartjs-adapter-date-fns";
import { Line } from "react-chartjs-2";
import zoomPlugin from "chartjs-plugin-zoom";

const crosshairPlugin = {
    id: "crosshair",
    afterEvent(chart, args) {
        chart._crosshair = args.event.type === "mousemove" ? { x: args.event.x, y: args.event.y } : null;
    },
    afterDraw(chart) {
        const pos = chart._crosshair;
        if (!pos) return;
        const { ctx, chartArea: { left, right, top, bottom }, scales: { x, y } } = chart;

        ctx.save();
        ctx.setLineDash([4, 4]);
        ctx.strokeStyle = "rgba(156, 163, 175, 0.5)";
        ctx.lineWidth = 0.5;
        ctx.beginPath();
        ctx.moveTo(pos.x, top);
        ctx.lineTo(pos.x, bottom);
        ctx.moveTo(left, pos.y);
        ctx.lineTo(right, pos.y);
        ctx.stroke();
        ctx.setLineDash([]);

        ctx.font = "11px monospace";
        ctx.textBaseline = "middle";

        const priceValue = y.getValueForPixel(pos.y);
        const priceText = priceValue.toFixed(2);
        const priceWidth = ctx.measureText(priceText).width + 8;
        ctx.fillStyle = "#3b82f6";
        ctx.fillRect(right, pos.y - 10, priceWidth, 20);
        ctx.fillStyle = "#fff";
        ctx.textAlign = "left";
        ctx.fillText(priceText, right + 4, pos.y);

        const timeValue = x.getValueForPixel(pos.x);
        const timeText = new Date(timeValue).toLocaleTimeString([], { hour: "2-digit", minute: "2-digit" });
        const timeWidth = ctx.measureText(timeText).width + 8;
        ctx.fillStyle = "#3b82f6";
        ctx.textAlign = "center";
        ctx.textBaseline = "top";
        ctx.fillRect(pos.x - timeWidth / 2, bottom, timeWidth, 20);
        ctx.fillStyle = "#fff";
        ctx.fillText(timeText, pos.x, bottom + 4);

        ctx.restore();
    },
};

ChartJS.register(LineElement, PointElement, LinearScale, TimeScale, Tooltip, Filler, ScatterController, crosshairPlugin, zoomPlugin);

const MAX_POINTS = 200;

const dayStart = new Date();
dayStart.setHours(9, 0, 0, 0);
const dayEnd = new Date(dayStart);
dayEnd.setHours(17, 0, 0, 0);

const chartOptions = {
    responsive: true,
    maintainAspectRatio: false,
    animation: false,
    scales: {
        x: {
            type: "time",
            min: dayStart.getTime(),
            max: dayEnd.getTime(),
            time: {
                unit: "hour",
                displayFormats: { hour: "HH:mm" },
            },
            ticks: { color: "#9ca3af" },
            grid: { color: "#374151" },
        },
        y: {
            position: "right",
            grace: "10%",
            ticks: { color: "#9ca3af" },
            grid: { color: "#374151" },
        },
    },
    plugins: {
        legend: { display: false },
        tooltip: {
            enabled: true,
            filter: (tooltipItem) => tooltipItem.datasetIndex === 1,
            callbacks: {
                label: (context) => {
                    const point = context.raw;
                    return point.label || `Price: ${point.y.toFixed(2)}`;
                },
            },
        },
        zoom: {
            pan: {
                enabled: true,
                mode: "x",
            },
            zoom: {
                wheel: { enabled: true },
                pinch: { enabled: true },
                mode: "x",
            },
            limits: {
                x: { min: dayStart.getTime(), max: dayEnd.getTime() },
            },
        },
    },
};

const initialData = {
    datasets: [
        {
            label: "Price",
            data: [],
            showLine: false,
            backgroundColor: "#3b82f6",
            borderColor: "#3b82f6",
            pointRadius: 1.5,
            fill: false,
        },
        {
            label: "Markers",
            data: [],
            type: "scatter",
            backgroundColor: (ctx) => {
                const age = Date.now() - (ctx.raw?.addedAt ?? 0);
                if (age < 200) return "rgba(239, 68, 68, 1)";
                if (age < 400) return "rgba(239, 68, 68, 0.85)";
                return "rgba(239, 68, 68, 0.7)";
            },
            borderColor: "#ef4444",
            pointRadius: (ctx) => {
                const age = Date.now() - (ctx.raw?.addedAt ?? 0);
                if (age < 200) return 12;
                if (age < 400) return 8;
                return 5;
            },
            pointStyle: "circle",
            showLine: false,
        },
    ],
};

function addMarker(chartRef, price, label) {
    const chart = chartRef.current;
    if (!chart) return;

    const now = Date.now();
    chart.data.datasets[1].data.push({ x: now, y: price, label, addedAt: now });
    chart.update("none");
    setTimeout(() => chart.update("none"), 200);
    setTimeout(() => chart.update("none"), 400);
}

function addPoint(chartRef, price) {
    const chart = chartRef.current;
    if (!chart) return;

    const dataset = chart.data.datasets[0];
    dataset.data.push({ x: Date.now(), y: price });

    if (dataset.data.length > MAX_POINTS) {
        dataset.data.shift();
    }

    chart.update("none");
}

function useMockMarketData(chartRef) {
    const priceRef = useRef(100);

    let count = 0;

    useEffect(() => {
        const interval = setInterval(() => {
            count++;
            console.log("Updating market data, count:", count);
            priceRef.current += (Math.random() - 0.5) * 2;
            if (count % 20 === 0) {
                addMarker(chartRef, priceRef.current, "Buy signal");
            }
            addPoint(chartRef, priceRef.current);
        }, 1000);

        return () => clearInterval(interval);
    }, [chartRef]);
}

function MarketData() {
    const chartRef = useRef(null);
    useMockMarketData(chartRef);

    return (
        <div className="flex flex-col w-full h-screen p-4 bg-gray-950">
            <h1 className="text-white text-xl mb-4">Market Data</h1>
            <div className="flex-1 min-h-0">
                <Line ref={chartRef} data={initialData} options={chartOptions} />
            </div>
        </div>
    );
}

export default MarketData;
