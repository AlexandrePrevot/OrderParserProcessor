function createScriptSubmit(title, content, summary, user) {
    return {
        title,
        content,
        summary,
        user,
        active : false
    };
}

export default createScriptSubmit;