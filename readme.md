# Simple Mail Client

This project is a simple mail client application implemented in C and Kotlin, designed to send and receive emails using the SMTP and POP3 protocols. The application supports basic email functionalities, including sending emails with authentication and retrieving email messages from the server.

---

## Features
- **SMTP Email Sending:**
    - Supports authentication using Base64 encoding.
    - Compliant with RFC 2822 email formatting standards.
    - Sends email with headers like `Date`, `MIME-Version`, and `Content-Type`.
    - Logs detailed SMTP commands and responses for debugging.

- **POP3 Email Retrieval:**
    - Connects to a POP3 server to fetch the list of emails.
    - Retrieves and displays email content.
    - Logs detailed POP3 commands and responses for debugging.

- **Base64 Encoding:**
    - Implements a Base64 encoding utility in C for authentication.
    - Provides support for encoding login credentials.

- **JNI Integration:**
    - Connects the C implementation with a Kotlin-based Android frontend via JNI.
    - Supports retrieval and display of emails within the Android application.

- **Environment Variable Support:**
    - Sensitive data such as server address, login, and password are now loaded from environment variables to enhance security.

---

## Technologies Used
- **Programming Languages:** C, Kotlin
- **Network Protocols:** SMTP, POP3
- **Frameworks:** Android SDK for the frontend
- **Build Tools:** CMake, Gradle

---

## Installation
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/username/simple-mail-client.git
   cd simple-mail-client
   ```

2. **Set Environment Variables:**
   ```bash
   export POP3_SERVER="your.pop3.server"
   export POP3_PORT=110
   export POP3_LOGIN="your_login"
   export POP3_PASS="your_password"
   export SMTP_SERVER="your.smtp.server"
   export SMTP_PORT=25
   export SMTP_LOGIN="your_login"
   export SMTP_PASS="your_password"
   ```

3. **Build the Project:**
    - Open the project in Android Studio.
    - Sync Gradle and build the project.

4. **Run the Application:**
    - Connect an Android device or start an emulator.
    - Deploy the application from Android Studio.

---

## Usage
### Sending Emails
1. Enter the sender's email address, recipient's email address, subject, and body in the provided fields.
2. Click the "Send Email" button.
3. Check the logs for command responses and verify successful email delivery.

### Retrieving Emails
1. Click the "Get Emails" button to fetch the list of emails from the POP3 server.
2. Select an email from the list to view its content.
3. Use the "Back" button to return to the list view.

---

## Project Structure
```
.
├── app/src/main/cpp
│   ├── smtp.c                # SMTP implementation
│   ├── pop3.c                # POP3 implementation
│   ├── utils.c               # Utility functions (Base64, socket handling)
│   ├── include
│   │   ├── smtp.h            # SMTP header
│   │   ├── pop3.h            # POP3 header
│   │   ├── utils.h           # Utility header
│   ├── native-lib.cpp        # JNI bindings
├── app/src/main/java
│   ├── com.pavelavl.simple_mail
│   │   ├── MainActivity.kt   # Android frontend
│   │   ├── EmailAdapter.kt   # Email list adapter
│   ├── res/layout
│   │   ├── activity_main.xml # UI layout
├── CMakeLists.txt            # CMake configuration
└── README.md                 # Project description
```

---

## Requirements
- Android Studio (latest version)
- Android SDK and NDK
- CMake for building native code

---

## Troubleshooting
### Common Issues
1. **SMTP Errors:**
    - Ensure the SMTP server supports authentication and allows email relaying.
    - Verify email format compliance with RFC 2822.

2. **POP3 Connection Issues:**
    - Confirm server settings and credentials.
    - Check if the server requires SSL/TLS (not currently supported).

3. **Base64 Encoding Errors:**
    - Validate the encoding function using sample inputs.

4. **Environment Variable Errors:**
    - Ensure all required environment variables are set correctly.
    - Restart the application after modifying environment variables.
