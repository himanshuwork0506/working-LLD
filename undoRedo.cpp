new word added - add in undo stack and clear redo stack - if undo executed, push to redo stack - if redo executed, push back to undo stack

1. Code Flow
Initialize Editor State
Execute Text Command
Update History Stacks
Undo Last Action
Redo Reverted Action

2. Classes
ICommand: Action Interface - execute, undo
ITextDocument: Editor Interface - append, erase, getText
TextEditor: Document Receiver - append, erase, getText
AppendCommand: Concrete Action  - execute, undo
CommandManager: History Controller - execute, undo, redo
Logger: Output Formatter - print

3. Requirements
Functional:
Append Text Data
Undo Previous Action
Redo Reverted Action

Non-Functional:
Command Pattern Compliant
Memory Efficient Stacks

C++
#include <iostream>
#include <stack>
#include <memory>
#include <string>

using namespace std;

// Defines the standard contract for executable and reversible actions.
class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~ICommand() = default;
};

// Defines the standard contract for the text document receiver.
class ITextDocument {
public:
    virtual void append(const string& str) = 0;
    virtual void erase(size_t length) = 0;
    virtual string getText() const = 0;
    virtual ~ITextDocument() = default;
};

// Concrete receiver that maintains the actual string state.
class TextEditor : public ITextDocument {
private:
    string text;
public:
    void append(const string& str) override { text += str; }
    
    void erase(size_t length) override { 
        if (length <= text.length()) text.erase(text.length() - length); 
    }
    
    string getText() const override { return text; }
};


// Concrete command encapsulating the append operation and its exact inverse.
class AppendCommand : public ICommand {
private:
    ITextDocument& doc; 
    string str;
public:
    AppendCommand(ITextDocument& d, string s) : doc(d), str(s) {}
    
    void execute() override { doc.append(str); }
    
    void undo() override { doc.erase(str.length()); }
};

// Invoker that manages the history of commands to support undo and redo operations.
class CommandManager {
private:
    stack<shared_ptr<ICommand>> undoStack, redoStack;

public:
    // Executes a new command and clears the redo history.
    void execute(shared_ptr<ICommand> cmd) {
        cmd->execute();
        undoStack.push(cmd);
        redoStack = {}; 
    }

    // Reverts the most recent command and pushes it to the redo stack.
    void undo() {
        if (undoStack.empty()) return;
        auto cmd = undoStack.top();
        undoStack.pop();
        
        cmd->undo();
        redoStack.push(cmd);
    }

    // Re-applies the most recently undone command.
    void redo() {
        if (redoStack.empty()) return;
        auto cmd = redoStack.top();
        redoStack.pop();
        
        cmd->execute();
        undoStack.push(cmd);
    }
};

// Handles terminal output formatting to separate UI from business logic.
class Logger {
public:
    static void print(const ITextDocument& doc, const string& action) {
        cout << "[" << action << "] -> Current Text: '" << doc.getText() << "'\n";
    }
};

// Application entry point demonstrating the command pattern lifecycle.
int main() {
    TextEditor editor;
    CommandManager manager;

    // 2. Execute Text Command
    manager.execute(make_shared<AppendCommand>(editor, "Hello"));
    Logger::print(editor, "Executed 'Hello'");

    manager.execute(make_shared<AppendCommand>(editor, " World"));
    Logger::print(editor, "Executed ' World'");

    // 4. Undo Last Action
    manager.undo();
    Logger::print(editor, "Undo");

    // 5. Redo Reverted Action
    manager.redo();
    Logger::print(editor, "Redo");

    return 0;
}