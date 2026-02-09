import { useEffect, useState } from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import Scripts from "./scripts/Scripts"
import MarketData from "./market-data/MarketData"
import Book from "./book/Book";
import SideBar from "./SideBar";
import NotificationBar from "./NotificationBar";
import NotFound from "./NotFound";

function MainLayout() {
    const [notifications, setNotifications] = useState([]);

    useEffect(() => {
        const ws = new WebSocket("ws://localhost:8000/ws");

        ws.onmessage = (event) => {
            const newNotification = {
                id: Date.now(),
                message: event.data,
                timestamp: new Date(),
            };
            setNotifications(prev => [newNotification, ...prev]);
        };

        ws.onerror = (error) => {
            console.error("WebSocket error:", error);
        };

        ws.onclose = () => {
            console.log("WebSocket connection closed");
        };

        return () => ws.close();
    }, []);

    const removeNotification = (id) => {
        setNotifications(prev => prev.filter(n => n.id !== id));
    };

    return (
        <BrowserRouter>
            <div className="flex">
                <SideBar />
                <Routes>
                    <Route path="book" element={<Book />} />
                    <Route path="marketdata" element={<MarketData />} />
                    <Route path="scripts" element={<Scripts />} />
                    <Route path="*" element={<NotFound />} />
                </Routes>
                <NotificationBar
                    notifications={notifications}
                    onRemove={removeNotification}
                />
            </div>
        </BrowserRouter>
    );
}

export default MainLayout
