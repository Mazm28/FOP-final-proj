# Tig - A Simple Version Control System

**Tig** is a lightweight version control system (VCS) inspired by Git. It provides basic functionality for tracking changes in your files, committing them, branching, tagging, and more. Tig is designed to be simple and easy to use, making it a great tool for small projects or learning how version control systems work.

---

## Features

- **Initialize a Repository**: Create a new Tig repository to start tracking changes.
- **Track Changes**: Add files to the staging area and commit them.
- **Branching**: Create and switch between branches to manage different lines of development.
- **Tagging**: Mark specific points in your project's history with tags.
- **Logs**: View the commit history and filter by branch, author, or message.
- **Checkout**: Switch between commits, branches, or tags.
- **Grep**: Search for specific content in files across commits.
- **Undo Changes**: Reset or undo changes in the staging area.

---

## Installation

Tig is written in C and can be compiled on any system with a C compiler (e.g., GCC).

### Prerequisites
- GCC (GNU Compiler Collection)
- Windows (for Windows-specific functions like `SetCurrentDirectory`)

### Steps
1. Clone the repository or download the source code.
   ```bash
   git clone https://github.com/yourusername/tig.git
   cd tig
   ```
2. Compile the code using GCC.
   ```bash
   gcc tig.c -o tig
   ```
3. Run the executable.
   ```bash
   ./tig
   ```

---

## Usage

### Commands

#### Initialize a Repository
```bash
tig init
```
Initializes a new Tig repository in the current directory.

#### Configure User Information
```bash
tig config -global user.name "Your Name"
tig config -global user.email "your.email@example.com"
```
Sets the user's name and email for commits.

#### Add Files to Staging Area
```bash
tig add <file1> <file2>
```
Adds files to the staging area for the next commit.

#### Commit Changes
```bash
tig commit -m "Your commit message"
```
Commits the changes in the staging area with a message.

#### View Status
```bash
tig status
```
Shows the status of files in the working directory and staging area.

#### Create a Branch
```bash
tig branch <branch_name>
```
Creates a new branch.

#### Switch to a Branch or Commit
```bash
tig checkout <branch_name_or_commit_id>
```
Switches to the specified branch or commit.

#### View Commit History
```bash
tig log
```
Displays the commit history.

#### Create a Tag
```bash
tig tag <tag_name> -m "Tag message"
```
Creates a tag at the current commit.

#### Search in Files
```bash
tig grep <file> <word>
```
Searches for a specific word in a file.

#### Reset Changes
```bash
tig reset <file>
```
Removes a file from the staging area.

#### Undo Last Add
```bash
tig reset -undo
```
Undoes the last `add` operation.

---

## Example Workflow

1. Initialize a repository:
   ```bash
   tig init
   ```

2. Add files to the staging area:
   ```bash
   tig add file1.txt file2.txt
   ```

3. Commit the changes:
   ```bash
   tig commit -m "Initial commit"
   ```

4. Create a new branch:
   ```bash
   tig branch feature-branch
   ```

5. Switch to the new branch:
   ```bash
   tig checkout feature-branch
   ```

6. Make changes and commit them:
   ```bash
   tig add file3.txt
   tig commit -m "Added file3.txt"
   ```

7. View the commit history:
   ```bash
   tig log
   ```

8. Create a tag:
   ```bash
   tig tag v1.0 -m "First release"
   ```

9. Search for a word in a file:
   ```bash
   tig grep file1.txt "search_word"
   ```

---

## File Structure

- **tig/**: The hidden directory where Tig stores its data.
  - `configname.txt`: Stores the user's name.
  - `configmail.txt`: Stores the user's email.
  - `staging area.txt`: Tracks files in the staging area.
  - `commit history.txt`: Stores the commit history.
  - `tags.txt`: Stores tag information.
  - `branches/`: Contains branch-specific data.
  - `commits/`: Stores commit snapshots.
  - `staging/`: Tracks staged files.

---

## Limitations

- Tig is a simple VCS and does not support advanced features like merging, rebasing, or remote repositories.
- It is currently designed for Windows due to the use of Windows-specific functions like `SetCurrentDirectory`.

---

## Contributing

Contributions are welcome! If you'd like to improve Tig, feel free to open an issue or submit a pull request.

---

## License

Tig is open-source software licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- Inspired by Git.
- Built for educational purposes.

---

Enjoy using Tig! 🚀
