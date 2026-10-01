# Argument Passing

## Project Title

**Argument Passing — Operating Systems and Systems Programming**

A web-based application that demonstrates argument passing in C using `argc` and `argv`, with a Python backend and web frontend.

## Team Members

| S.No | Name | Roll Number |
|------|------|-------------|
| 1 | V Ashish Reddy | 2520030415 |
| 2 | Shaik Juneeth | 2520030435 |
| 3 | Venkata Mahith | 2520030497 |

## System Architecture

```text
                ┌─────────────────────┐
                │       User          │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │   Web Frontend      │
                │   HTML / CSS / JS   │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │   Python Backend    │
                │      Server         │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │      C Program      │
                │  Argument Passing   │
                │      argc / argv    │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │       Output        │
                │ Arguments / Results │
                └─────────────────────┘
cat PROJECT/Argument_Passing_WebApp/README.md
    ## Objectives

- To understand argument passing in C.
- To demonstrate the use of `argc` and `argv`.
- To provide a web-based interface for the application.
- To connect the frontend with the Python backend.
- To execute the C program through the backend.
- To display the processed arguments and results.

## Expected Output

The application accepts arguments through the web interface and sends them to the backend.

The backend executes the C program with the provided arguments and displays the result on the webpage.

The output includes the number of arguments received, the arguments passed, and the final execution result.

## Conclusion

The **Argument Passing** project demonstrates how command-line arguments are passed to a C program using `argc` and `argv`. It combines C programming, Python backend development, and web technologies to provide an interactive demonstration of an Operating Systems concept.

## GitHub Repository

https://github.com/ashishreddy27/KLH-CSE-2026-2027-Team-18-Argument_Passing
