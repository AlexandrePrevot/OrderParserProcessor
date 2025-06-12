import { useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";

function List({AlgoScripList, onSelectedScript}) {
    return (
        <div className="itemwrap p-4">
            {AlgoScripList.map((AlgoScript) => (<button key={AlgoScript.id} onClick={() => onSelectedScript(AlgoScript)} className="px-4 py-2 rounded">
                {AlgoScript.name}
            </button>))}
        </div>
    );
}

function AlgoScript({selectedScript}) {
    return (
    <div>
        {selectedScript ? 
        (
            <pre>{selectedScript.content}</pre>
        ) :
        (
            <div>Select a file to view its content</div>
        )}
    </div>)
}

function ScriptsList() {

    const AlgoScriptList = [
        { id: 1, name: 'File 1', content: 'This is the content of File 1.' },
        { id: 2, name: 'File 2', content: 'This is the content of File 2.\nLine 2 of file 2.' },
        { id: 3, name: 'File 3', content: 'This is the content of File 3.' },
        { id: 4, name: 'Script1', content: 'Script1 actual content.' }
    ];

    const [selectedScript, onSelectedScript] = useState(null);

    return (
        <div>
            <PanelGroup autoSaveId="example" direction="horizontal">

                <Panel defaultSize={25}>
                    <div style={{ minHeight: 600 }} className="listlayout w-75">
                        <List AlgoScripList = {AlgoScriptList} onSelectedScript={onSelectedScript}/>
                    </div>
                </Panel>

                <PanelResizeHandle className="w-2 bg-blue-300 hover:bg-blue-500 transition-colors duration-200 cursor-col-resize" />

                <Panel>
                    <PanelGroup direction="vertical">

                        <Panel>
                            <div>
                                <AlgoScript selectedScript={selectedScript}/>
                            </div>
                        </Panel>

                        <PanelResizeHandle className="h-2 bg-blue-300"/>

                        <Panel>
                            <div>
                                <textarea
                                    className="w-screen h-screen"
                                    placeholder="Summary of the script..."
                                />
                            </div>
                        </Panel>

                    </PanelGroup>
                </Panel>

            </PanelGroup>
        </div>
    );
}

export default ScriptsList;