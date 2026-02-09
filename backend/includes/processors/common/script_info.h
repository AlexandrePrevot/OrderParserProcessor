#pragma once

#include <string>

class ScriptInfo {
 public:
  static ScriptInfo& GetInstance() {
    static ScriptInfo instance;
    return instance;
  }

  ScriptInfo(const ScriptInfo&) = delete;
  ScriptInfo& operator=(const ScriptInfo&) = delete;
  ScriptInfo(ScriptInfo&&) = delete;
  ScriptInfo& operator=(ScriptInfo&&) = delete;

  void SetUsername(const std::string& username) { username_ = username; }
  const std::string& GetUsername() const { return username_; }

  void SetScriptTitle(const std::string& script_title) {
    script_title_ = script_title;
  }
  const std::string& GetScriptTitle() const { return script_title_; }

 private:
  ScriptInfo() = default;

  std::string username_;
  std::string script_title_;
};
