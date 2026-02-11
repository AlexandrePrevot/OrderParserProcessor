import { useState, useEffect, useRef } from 'react';

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
    const formatTime = (date) => {
        return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
    };

    return (
        <div
            className="p-3 rounded mb-2 relative"
            style={{
                animation: isNew ? 'slideIn 0.3s ease-out' : 'none',
                backgroundColor: isNew ? '#1e3a5f' : '#1f2937',
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
            <div className="text-xs text-blue-400 mb-1 font-semibold">
                {notification.script_title}
            </div>
            <div className="text-sm">
                {notification.message}
            </div>
        </div>
    );
}

function NotificationBar({ notifications = [], onRemove, collapsed, onToggleCollapse }) {
    const [expanded, setExpanded] = useState(false);
    const [newIds, setNewIds] = useState(new Set());
    const prevIdsRef = useRef(new Set());

    useEffect(() => {
        const currentIds = new Set(notifications.map(n => n.id));
        const newlyAdded = new Set();

        for (const id of currentIds) {
            if (!prevIdsRef.current.has(id)) {
                newlyAdded.add(id);
            }
        }

        prevIdsRef.current = currentIds;

        if (newlyAdded.size > 0) {
            setNewIds(newlyAdded);
            const timer = setTimeout(() => setNewIds(new Set()), 600);
            return () => clearTimeout(timer);
        }
    }, [notifications]);

    const visibleCount = 10;
    const hasMore = notifications.length > visibleCount;
    const displayedNotifications = expanded ? notifications : notifications.slice(0, visibleCount);

    if (collapsed) {
        return (
            <div style={{
                width: 40,
                position: 'fixed',
                right: 0,
                top: 0,
                height: '100vh',
                backgroundColor: '#111827',
                color: 'white',
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                borderLeft: '1px solid #374151',
            }}>
                <button
                    onClick={onToggleCollapse}
                    style={{
                        background: 'none',
                        border: 'none',
                        color: 'white',
                        cursor: 'pointer',
                        padding: '12px 0',
                        fontSize: '18px',
                        position: 'relative',
                    }}
                    title="Open notifications"
                >
                    🔔
                    {notifications.length > 0 && (
                        <span style={{
                            position: 'absolute',
                            top: 8,
                            right: -2,
                            backgroundColor: '#ef4444',
                            borderRadius: '50%',
                            width: 16,
                            height: 16,
                            fontSize: '10px',
                            display: 'flex',
                            alignItems: 'center',
                            justifyContent: 'center',
                        }}>
                            {notifications.length}
                        </span>
                    )}
                </button>
            </div>
        );
    }

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
                    <button
                        onClick={onToggleCollapse}
                        style={{
                            background: 'none',
                            border: 'none',
                            color: '#9ca3af',
                            cursor: 'pointer',
                            fontSize: '18px',
                        }}
                        title="Collapse"
                    >
                        »
                    </button>
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
                                    isNew={newIds.has(notification.id)}
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
