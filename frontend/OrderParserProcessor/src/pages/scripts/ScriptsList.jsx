import { useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";
import styled from "styled-components";

import createScriptSubmit from "../../structures/ScriptSubmit"
import { handleScriptRequest, handleActivateScript } from "../../backend-api/ScriptRequestHandler";
import { Editor } from "@monaco-editor/react";

//Button style got from https://react.school/ui/button
const theme = {
    blue: {
        default: "#3f51b5",
        hover: "#283593",
    },
    pink: {
        default: "#e91e63",
        hover: "#ad1457",
    },
};

const Button = styled.button`
    background-color: ${(props) => theme[props.theme].default};
    color: white;
    padding: 5px 15px;
    border-radius: 5px;
    outline: 0;
    border: 0; 
    text-transform: uppercase;
    margin: 10px 0px;
    cursor: pointer;
    box-shadow: 0px 2px 2px lightgray;
    transition: ease background-color 250ms;
    &:hover {
      background-color: ${(props) => theme[props.theme].hover};
    }
    &:disabled {
      cursor: default;
      opacity: 0.7;
    }
  `;

Button.defaultProps = {
    theme: "blue",
};

const ButtonToggle = styled(Button)`
    opacity: 0.7;
    ${({ active }) =>
        active &&
        `
      opacity: 1; 
    `}
  `;

const Tab = styled.button`
    padding: 10px 30px;
    cursor: pointer;
    opacity: 0.6;
    background: white;
    border: 0;
    outline: 0;
    border-bottom: 2px solid transparent;
    transition: ease border-bottom 250ms;
    ${({ active }) =>
        active &&
        `
      border-bottom: 2px solid black;
      opacity: 1;
    `}
  `;




function List({ AlgoScripList, onSelectedScript }) {
    const handleSelectScript = (AlgoScript) => {
        onSelectedScript(AlgoScript);
    };
    return (
        <div className="flex-wrap p-4 overflow-y-auto" style={{ maxHeight: "50vh" }}>
            {AlgoScripList.map((AlgoScript) => (<Button onClick={() => handleSelectScript(AlgoScript)} className="px-4 py-2 rounded">
                {AlgoScript.title}
            </Button>))}
        </div>
    );
}

function CodeEditor({ value, onChange }) {
    return (
        <Editor
            height="80%"
            value={value}
            onChange={(newValue) => onChange(newValue || "")}
            theme="vs-light"
            options={{
                fontSize: 14,
                minimap: { enabled: false },
                suggestOnTriggerCharacters: false,
                quickSuggestions: false,
                wordBasedSuggestions: false,
                parameterHints: false,
                tabCompletion: "off",
                acceptSuggestionOnEnter: "off"
            }}
        />
    );
}


function AlgoScript({ selectedScript, setScriptList, scriptList}) {
    const [content, setContent] = useState('');
    const [summary, setSummary] = useState('');
    const [title, setTitle] = useState('');
    
    // monaco editor just pass the text when changing
    const handleContentChange = (val) => {
        if (selectedScript) {
            selectedScript.content = val;
        }
        setContent(val)
    };

    const handleSummaryChange = (e) => {
        if (selectedScript) {
            selectedScript.summary = e.target.value;
        }
        setSummary(e.target.value);
    };

    const handleTitleChange = (e) => {
        if (selectedScript) {
            selectedScript.title = e.target.value;
        }
        setTitle(e.target.value);
    };

    const handleSave = () => {
        if (selectedScript) {
            setScriptList([...scriptList]);
            handleScriptRequest(selectedScript);
        }
    };

    const handleToggleActivation = async () => {
        if (!selectedScript) return;
        try {
            const result = await handleActivateScript(
                selectedScript.user,
                selectedScript.title
            );
            selectedScript.active = result.active;
            setScriptList([...scriptList]);
        } catch (error) {
            console.error("Activation toggle failed:", error.message);
        }
    };

    // when adding too many lines, can't see the title anymore
    return (
        <PanelGroup direction="vertical" style={{ height: "100%" }}>
            <Panel style={{ height: "100%" }}>
                <div style={{ height: "100%" }}>
                    <div style={{display: 'flex',  justifyContent:'center', alignItems:'center'}}>
                        {
                            selectedScript ?
                            (
                                <textarea  maxLength="30" placeholder="Script Title !" value={selectedScript.title} onChange={handleTitleChange} />
                            ) :
                            (
                                <textarea placeholder="Create your script !" />
                            )
                        }
                    </div>
                    <div className="flex-wrap p-4">
                        <Button onClick={handleSave}>
                            Save
                        </Button>
                        <Button onClick={handleToggleActivation} theme={selectedScript?.active ? "pink" : "blue"}>
                            {selectedScript?.active ? "Deactivate" : "Activate"}
                        </Button>
                    </div>
                    <div style={{ height: "80%" }}>
                        {selectedScript ?
                            (
                                <CodeEditor className="w-screen h-screen" value={selectedScript.content} onChange={handleContentChange} />
                            ) :
                            (
                                <div>Select a file to view its content</div>
                            )}
                    </div>
                </div>
            </Panel>

            <PanelResizeHandle className="h-2 bg-blue-300" />

            <Panel>
                <div>
                    {
                        selectedScript ?
                            (
                                <textarea className="w-screen h-screen" placeholder="Summary of the script..." value={selectedScript.summary} onChange={handleSummaryChange} />
                            ) :
                            (
                                <textarea className="w-screen h-screen" placeholder="Summary of the script..." />
                            )
                    }
                </div>
            </Panel>
        </PanelGroup>
    )
}

function ScriptsList() {

    const [scriptList, setScriptList] = useState([
        createScriptSubmit("Low Price trigger", "if (price is low) SendBuyOrder(order)", "when the price is low, it will send a buy order", "Jean Baptiste "),
        createScriptSubmit("High Price trigger", "if (price is high) SendSellOrder(order)", "when the price is high, it will send a sell order", "Jean Baptiste"),
        createScriptSubmit("Short Position Send Buy", "if (Position(MYUSER) is short) SendBuyOrder(order)", "We do not accept to be short for some type of instruments", "Jean Baptiste"),
    ]);


    const [selectedScript, onSelectedScript] = useState(null);

    const handleScriptCreation = () => {
        let new_script = createScriptSubmit("New Script", "New Script !", "New Script Summary !", "Jean Baptiste")
        setScriptList([...scriptList, new_script]);
    };

    return (
        <div className="w-75 h-screen">
            <PanelGroup autoSaveId="example" direction="horizontal">

                <Panel defaultSize={25}>
                    <div style={{ minHeight: 600 }} className="listlayout w-75">
                        <List AlgoScripList={scriptList} onSelectedScript={onSelectedScript} />
                    </div>
                    <div>
                        <Button onClick={handleScriptCreation}>
                            Create a new script
                        </Button>
                    </div>
                </Panel>

                <PanelResizeHandle className="w-2 bg-blue-300 hover:bg-blue-500 transition-colors duration-200 cursor-col-resize" />

                <Panel>
                    <AlgoScript selectedScript={selectedScript} setScriptList={setScriptList} scriptList={scriptList}  />
                </Panel>

            </PanelGroup>
        </div>
    );
}

export default ScriptsList;