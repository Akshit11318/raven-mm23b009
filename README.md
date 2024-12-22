# Project: Raven-mm23b009

Welcome to the **Raven-mm23b009** repository! This project contains various components related to C++ programming challenges and concepts. Below is an overview of the files and their contents to help you navigate the repository.

## File Structure

```
raven-mm23b009/
├── Q2.md
├── Q3-mm23b009.cpp
├── Q3.md
├── Q4.md
├── README.md
```

### 1. [Q2.md](./Q2.md)

**Description:**
- This file provides detailed documentation for the codebase named "Raven."
- It explains the architecture, purpose, and functionality of the related code components.

---

### 2. [Q3-mm23b009.cpp](./Q3-mm23b009.cpp)

**Description:**
- A C++ implementation of a robust **Market Data Publishing System.**
- Supports equity and bond instruments with flexible subscription models.

#### Key Highlights:
- **Dual Publisher System:** Handles both equity (IDs: 0-999) and bond (IDs: 1000-1999) instruments.
- **Subscription Models:**
  - Paid Subscribers: Unlimited data requests.
  - Free Subscribers: Limited to 100 successful requests.
- **Data Management:** Real-time updates, subscription validation, and access control.
- **Extensibility:**
  - Abstract base classes for publishers and subscribers.
  - Designed for easy integration of new types.

For a detailed explanation, refer to [Q3.md](./Q3.md).

---

### 3. [Q3.md](./Q3.md)

**Description:**
- Provides an in-depth overview of the **Market Data Publishing System.**
- Explains the features, architecture, and principles applied to the implementation.

#### Topics Covered:
- Dual Publisher System
- Subscription Models
- Data Management
- Extensible Architecture

---

### 4. [Q4.md](./Q4.md)

**Description:**
- Discusses the use of `std::unique_ptr` in C++.
- Explains its role in memory management, ensuring resource safety through RAII (Resource Acquisition Is Initialization).
- Highlights best practices and examples of using `std::unique_ptr` in modern C++.

---

### 5. [README.md](./README.md)

**Description:**
- The current file provides an overview of the project, highlighting the purpose and contents of each file in the repository.
- Contains links to all major components for easy access.

---

### Navigation
- [Q2.md](./Q2.md)
- [Q3-mm23b009.cpp](./Q3-mm23b009.cpp)
- [Q3.md](./Q3.md)
- [Q4.md](./Q4.md)

We hope you find this repository helpful and insightful!
