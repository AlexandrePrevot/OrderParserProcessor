import { useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";
import styled from "styled-components";

import createScriptSubmit from "../../structures/ScriptSubmit"
import { handleScriptRequest, handleActivateScript } from "../../backend-api/ScriptRequestHandler";
import { Editor } from "@monaco-editor/react";

const theme = {
    blue: {
        default: "#3b82f6",
        hover: "#2563eb",
    },
    green: {
        default: "#10b981",
        hover: "#059669",
    },
    red: {
        default: "#ef4444",
        hover: "#dc2626",
    },
};

const Button = styled.button`
    background-color: ${(props) => theme[props.theme].default};
    color: white;
    padding: 6px 16px;
    border-radius: 6px;
    outline: 0;
    border: 0;
    text-transform: uppercase;
    font-size: 13px;
    font-weight: 500;
    letter-spacing: 0.5px;
    cursor: pointer;
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


function List({ AlgoScripList, selectedScript, onSelectedScript }) {
    return (
        <div style={{
            display: "flex",
            flexDirection: "column",
            height: "100%",
            backgroundColor: "#111827",
        }}>
            <div style={{
                padding: "16px",
                borderBottom: "1px solid #374151",
                fontWeight: "bold",
                color: "white",
                fontSize: "14px",
                letterSpacing: "0.5px",
            }}>
                Scripts
            </div>
            <div style={{ flex: 1, overflowY: "auto", padding: "8px" }}>
                {AlgoScripList.map((script) => {
                    const isSelected = selectedScript === script;
                    return (
                        <div
                            key={script.title}
                            onClick={() => onSelectedScript(script)}
                            style={{
                                padding: "10px 12px",
                                marginBottom: "4px",
                                borderRadius: "6px",
                                cursor: "pointer",
                                backgroundColor: isSelected ? "#374151" : "transparent",
                                borderLeft: isSelected ? "3px solid #3b82f6" : "3px solid transparent",
                                color: isSelected ? "white" : "#d1d5db",
                                transition: "all 150ms ease",
                                display: "flex",
                                alignItems: "center",
                                gap: "8px",
                                fontSize: "13px",
                            }}
                            onMouseEnter={(e) => {
                                if (!isSelected) e.currentTarget.style.backgroundColor = "#1f2937";
                            }}
                            onMouseLeave={(e) => {
                                if (!isSelected) e.currentTarget.style.backgroundColor = "transparent";
                            }}
                        >
                            <span style={{
                                width: 8,
                                height: 8,
                                borderRadius: "50%",
                                backgroundColor: script.active ? "#10b981" : "#4b5563",
                                flexShrink: 0,
                            }} />
                            {script.title}
                        </div>
                    );
                })}
            </div>
            <div style={{ padding: "12px", borderTop: "1px solid #374151" }}>
            </div>
        </div>
    );
}

function CodeEditor({ value, onChange }) {
    return (
        <Editor
            height="100%"
            value={value}
            onChange={(newValue) => onChange(newValue || "")}
            theme="vs-dark"
            options={{
                fontSize: 14,
                minimap: { enabled: false },
                suggestOnTriggerCharacters: false,
                quickSuggestions: false,
                wordBasedSuggestions: false,
                parameterHints: false,
                tabCompletion: "off",
                acceptSuggestionOnEnter: "off",
                scrollBeyondLastLine: false,
                padding: { top: 12 },
            }}
        />
    );
}


function AlgoScript({ selectedScript, setScriptList, scriptList }) {
    const [content, setContent] = useState('');
    const [summary, setSummary] = useState('');
    const [title, setTitle] = useState('');

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

    if (!selectedScript) {
        return (
            <div style={{
                height: "100%",
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                color: "#6b7280",
                fontSize: "15px",
            }}>
                Select a script to start editing
            </div>
        );
    }

    return (
        <div style={{ height: "100%", display: "flex", flexDirection: "column" }}>
            <div style={{
                display: "flex",
                alignItems: "center",
                justifyContent: "space-between",
                padding: "10px 16px",
                backgroundColor: "#111827",
                borderBottom: "1px solid #374151",
            }}>
                <input
                    type="text"
                    maxLength="30"
                    placeholder="Script Title"
                    value={selectedScript.title}
                    onChange={handleTitleChange}
                    style={{
                        background: "transparent",
                        border: "none",
                        outline: "none",
                        color: "white",
                        fontSize: "16px",
                        fontWeight: 600,
                        width: "300px",
                        borderBottom: "1px solid transparent",
                        paddingBottom: "2px",
                        transition: "border-color 200ms",
                    }}
                    onFocus={(e) => e.target.style.borderBottomColor = "#3b82f6"}
                    onBlur={(e) => e.target.style.borderBottomColor = "transparent"}
                />
                <div style={{ display: "flex", gap: "8px" }}>
                    <Button onClick={handleSave} theme="blue">
                        Save
                    </Button>
                    <Button onClick={handleToggleActivation} theme="green">
                        Activate/Deactivate
                    </Button>
                </div>
            </div>

            <PanelGroup direction="vertical" style={{ flex: 1 }}>
                <Panel defaultSize={75}>
                    <div style={{ height: "100%", backgroundColor: "#1e1e1e" }}>
                        <CodeEditor value={selectedScript.content} onChange={handleContentChange} />
                    </div>
                </Panel>

                <PanelResizeHandle style={{
                    height: "4px",
                    backgroundColor: "#1f2937",
                    cursor: "row-resize",
                    transition: "background-color 200ms",
                }}
                    className="hover:bg-blue-500"
                />

                <Panel>
                    <div style={{
                        height: "100%",
                        display: "flex",
                        flexDirection: "column",
                        backgroundColor: "#111827",
                    }}>
                        <div style={{
                            padding: "8px 16px",
                            color: "#6b7280",
                            fontSize: "12px",
                            fontWeight: 600,
                            textTransform: "uppercase",
                            letterSpacing: "0.5px",
                            borderBottom: "1px solid #374151",
                        }}>
                            Description
                        </div>
                        <textarea
                            placeholder="Summary of the script..."
                            value={selectedScript.summary}
                            onChange={handleSummaryChange}
                            style={{
                                flex: 1,
                                width: "100%",
                                backgroundColor: "#1f2937",
                                color: "#d1d5db",
                                border: "none",
                                outline: "none",
                                padding: "12px 16px",
                                fontSize: "14px",
                                resize: "none",
                                fontFamily: "inherit",
                            }}
                        />
                    </div>
                </Panel>
            </PanelGroup>
        </div>
    );
}

function ScriptsList() {

    const [scriptList, setScriptList] = useState([
    ]);

    const [selectedScript, onSelectedScript] = useState(null);

    const handleScriptCreation = () => {
        let new_script = createScriptSubmit("New Script", "New Script !", "New Script Summary !", "Jean Baptiste")
        setScriptList([...scriptList, new_script]);
    };

    return (
        <div style={{ height: "100%" }}>
            <PanelGroup autoSaveId="scripts" direction="horizontal">

                <Panel defaultSize={22} minSize={15}>
                    <div style={{ height: "100%", display: "flex", flexDirection: "column" }}>
                        <div style={{ flex: 1, overflow: "hidden" }}>
                            <List
                                AlgoScripList={scriptList}
                                selectedScript={selectedScript}
                                onSelectedScript={onSelectedScript}
                            />
                        </div>
                        <div style={{
                            padding: "0 12px 12px",
                            backgroundColor: "#111827",
                        }}>
                            <Button onClick={handleScriptCreation} theme="blue" style={{ width: "100%" }}>
                                + New Script
                            </Button>
                        </div>
                    </div>
                </Panel>

                <PanelResizeHandle style={{
                    width: "4px",
                    backgroundColor: "#1f2937",
                    cursor: "col-resize",
                    transition: "background-color 200ms",
                }}
                    className="hover:bg-blue-500"
                />

                <Panel>
                    <AlgoScript selectedScript={selectedScript} setScriptList={setScriptList} scriptList={scriptList} />
                </Panel>

            </PanelGroup>
        </div>
    );
}

export default ScriptsList;
