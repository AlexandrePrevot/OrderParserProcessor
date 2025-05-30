function List() {
    const tags = ['Apple', 'Banana', 'Cherry', 'Date', 'Elderberry', 'Fig', 'Grape', 'Script1', 'Script2', 'Script3', 'Script4', 'Script2', 'Script3', 'Script4', 'Script2', 'Script3', 'Script4', 'Script2', 'Script3', 'Script4', 'Script2', 'Script3', 'Script4'];
    return (
        <div className="itemwrap p-4">
            {tags.map((tag, index) => (<button key={index} className="px-4 py-2 rounded">
                {tag}
            </button>))}
        </div>
    );
}

function ScriptsList() {
    return (
        <div style={{ minHeight: 600 }} className="listlayout w-75 border border-red-500">
            <List />
        </div>
    );
}

export default ScriptsList;