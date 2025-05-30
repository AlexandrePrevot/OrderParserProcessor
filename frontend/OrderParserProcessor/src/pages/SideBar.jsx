import { User, Book, BarChart2, Code } from 'lucide-react';
import { useNavigate } from "react-router-dom";

function SideBar() {
    const navigate = useNavigate();
    return <div style={{ minWidth: 200 }} className="h-screen bg-gray-900 text-white">
        <nav className="d-flex flex-column">
            <button onClick={() => navigate('/book')} className="flex items-center gap-2 px-4 py-2 hover:bg-gray-800 rounded">
                <Book size={18} /> Book
            </button>
            <button onClick={() => navigate('/marketdata')} className="flex items-center gap-2 px-4 py-2 hover:bg-gray-800 rounded">
                <BarChart2 size={18} /> Trade View
            </button>
            <button onClick={() => navigate('/scripts')} className="flex items-center gap-2 px-4 py-2 hover:bg-gray-800 rounded">
                <Code size={18} /> Scripts
            </button>
        </nav>
    </div>
}

export default SideBar;