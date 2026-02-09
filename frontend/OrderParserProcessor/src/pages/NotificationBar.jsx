import { useState, useEffect } from 'react';

const slideInKeyframes = `
@keyframes slideIn {
    from {
        opacity: 0;
        transform: translateX(20px);
    }
    to {
        opacity: 1;
        transform: translateX(0);
    }
}
`;

function NotificationItem({ notification, onRemove, isNew }) {
    const [animate, setAnimate] = useState(isNew);

    useEffect(() => {
        if (isNew) {
            const timer = setTimeout(() => setAnimate(false), 500);
            return () => clearTimeout(timer);
        }
    }, [isNew]);

    const formatTime = (date) => {
        return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
    };

    return (
        <div
            className="p-3 rounded mb-2 relative"
            style={{
                animation: animate ? 'slideIn 0.3s ease-out' : 'none',
                backgroundColor: animate ? '#1e3a5f' : '#1f2937',
                transition: 'background-color 0.5s ease',
            }}
        >
            <button
                onClick={() => onRemove(notification.id)}
                className="absolute top-2 right-2 text-gray-400 hover:text-white"
                style={{ background: 'none', border: 'none', cursor: 'pointer', fontSize: '16px' }}
            >
                ×
            </button>
            <div className="text-xs text-gray-400 mb-1">
                {formatTime(notification.timestamp)}
            </div>
            <div className="text-sm">
                {notification.message}
            </div>
        </div>
    );
}

function NotificationBar({ notifications = [], onRemove }) {
    const [expanded, setExpanded] = useState(false);
    const [newestId, setNewestId] = useState(null);

    useEffect(() => {
        if (notifications.length > 0) {
            setNewestId(notifications[0].id);
        }
    }, [notifications]);

    const visibleCount = 10;
    const hasMore = notifications.length > visibleCount;
    const displayedNotifications = expanded ? notifications : notifications.slice(0, visibleCount);

    return (
        <>
            <style>{slideInKeyframes}</style>
            <div style={{
                width: 280,
                position: 'fixed',
                right: 0,
                top: 0,
                height: '100vh',
                backgroundColor: '#111827',
                color: 'white',
                display: 'flex',
                flexDirection: 'column',
            }}>
                <div style={{
                    padding: '16px',
                    borderBottom: '1px solid #374151',
                    fontWeight: 'bold',
                    display: 'flex',
                    justifyContent: 'space-between',
                    alignItems: 'center',
                }}>
                    <span>Notifications ({notifications.length})</span>
                </div>

                <div style={{
                    flex: 1,
                    overflowY: 'auto',
                    padding: '12px',
                }}>
                    {notifications.length === 0 ? (
                        <div className="text-gray-500 text-center py-4">
                            No notifications
                        </div>
                    ) : (
                        <>
                            {displayedNotifications.map((notification) => (
                                <NotificationItem
                                    key={notification.id}
                                    notification={notification}
                                    onRemove={onRemove}
                                    isNew={notification.id === newestId}
                                />
                            ))}

                            {hasMore && !expanded && (
                                <button
                                    onClick={() => setExpanded(true)}
                                    style={{
                                        width: '100%',
                                        padding: '8px',
                                        backgroundColor: '#374151',
                                        border: 'none',
                                        borderRadius: '4px',
                                        color: 'white',
                                        cursor: 'pointer',
                                        marginTop: '8px',
                                    }}
                                >
                                    Show {notifications.length - visibleCount} more
                                </button>
                            )}

                            {expanded && hasMore && (
                                <button
                                    onClick={() => setExpanded(false)}
                                    style={{
                                        width: '100%',
                                        padding: '8px',
                                        backgroundColor: '#374151',
                                        border: 'none',
                                        borderRadius: '4px',
                                        color: 'white',
                                        cursor: 'pointer',
                                        marginTop: '8px',
                                    }}
                                >
                                    Show less
                                </button>
                            )}
                        </>
                    )}
                </div>
            </div>
        </>
    );
}

export default NotificationBar;
