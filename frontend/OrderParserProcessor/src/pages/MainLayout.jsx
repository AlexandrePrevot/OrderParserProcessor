import { BrowserRouter, Routes, Route } from 'react-router-dom';
import Scripts from "./scripts/Scripts"
import MarketData from "./market-data/MarketData"
import Book from "./book/Book";
import SideBar from "./SideBar";
import NotFound from "./NotFound";

function MainLayout() {
    return (
        <BrowserRouter>
            <div className ="flex">
                <SideBar />
                <Routes>
                    <Route path="book" element={<Book />} />
                    <Route path="marketdata" element={<MarketData />} />
                    <Route path="scripts" element={<Scripts />} />
                    <Route path="*" element={<NotFound />} />
                </Routes>
            </div>
        </BrowserRouter>
    );
}

export default MainLayout