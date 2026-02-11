import { useEffect, useRef, useState } from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import Scripts from "./scripts/Scripts"
import MarketData from "./market-data/MarketData"
import Book from "./book/Book";
import SideBar from "./SideBar";
import NotificationBar from "./NotificationBar";
import NotFound from "./NotFound";

function MainLayout() {
    const [notifications, setNotifications] = useState([]);
    const [notifCollapsed, setNotifCollapsed] = useState(false);
    const marketDataRef = useRef(null);

    useEffect(() => {
        const ws = new WebSocket("ws://localhost:8000/ws");

        ws.onmessage = (event) => {
            const obj = JSON.parse(event.data);

            if (obj.MessageType === "price_update") {
                marketDataRef.current?.addPoint(obj.price);
                return;
            }

            const newNotification = {
                id: Date.now(),
                message: obj.message,
                user: obj.user,
                script_title: obj.script_title,
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
                <div style={{ flex: 1, marginRight: notifCollapsed ? 40 : 280 }}>
                    <Routes>
                        <Route path="book" element={<Book />} />
                        <Route path="marketdata" element={<MarketData ref={marketDataRef} />} />
                        <Route path="scripts" element={<Scripts />} />
                        <Route path="*" element={<NotFound />} />
                    </Routes>
                </div>
                <NotificationBar
                    notifications={notifications}
                    onRemove={removeNotification}
                    collapsed={notifCollapsed}
                    onToggleCollapse={() => setNotifCollapsed(prev => !prev)}
                />
            </div>
        </BrowserRouter>
    );
}

export default MainLayout
