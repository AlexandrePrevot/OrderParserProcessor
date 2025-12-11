
CREATE TABLE ScriptSubmit (
    ScriptID        BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    Code            TEXT NOT NULL,
    Title           VARCHAR(255) NOT NULL,
    CreationDate    DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    Author          VARCHAR(255) NOT NULL,
    Active          BOOLEAN NOT NULL DEFAULT FALSE,
    ActivationDate  DATETIME NULL,
    Summary         TEXT NULL,
    LastModifDate   DATETIME NULL ON UPDATE CURRENT_TIMESTAMP
)