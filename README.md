# STM32-Powered IoT Security Project 🚀
## Giorgio Colella & Maria Caterina D'Aloia

An advanced IoT security project leveraging STM32 capabilities and AWS IoT Core to create a secure, connected environment for the future. This repository includes insights, setup instructions, and a demo, all tailored for professionals and enthusiasts in IoT security.

## 📜 Table of Contents
1. [Overview](#overview)
2. [Why IoT Security?](#why-iot-security)
3. [Components & Capabilities](#components--capabilities)
4. [AWS IoT Core Integration](#aws-iot-core-integration)
5. [Setup Guide](#setup-guide)
6. [Tools & Documentation](#tools--documentation)
7. [Demo](#demo)
8. [Resources](#resources)

---

## 📖 Overview

As IoT devices proliferate, the need for security becomes critical. With an estimated **29.42 billion IoT devices** by 2030, addressing potential threats to connected systems is essential. This project demonstrates IoT security solutions using **STM32U575ZIT6Q** and **AWS IoT Core**, focusing on secure data transmission, device authentication, and robust cloud management.

## 🌐 Why IoT Security?

- **Rising IoT Attacks:** IoT devices have become prime targets for cyber-attacks, making secure protocols essential.
- **Scalability:** AWS IoT Core supports scaling with cost-effective solutions, meeting the needs of millions of connected devices.
- **Customer-Centric Security**: Inspired by AWS’s principle
>“We’re not competitor obsessed, we’re customer obsessed”

This project prioritizes user security and data protection.

## 🔧 Components & Capabilities

### STM32U575ZIT6Q Security Features
- **Advanced Cryptography**: Built-in cryptographic capabilities for secure data handling.
- **TLS 1.2 Support**: Ensures data integrity and privacy over network connections.
- **Certificates & Keys**: Authentication mechanisms that verify device legitimacy.
  
### Why AWS IoT Core?
AWS IoT Core provides seamless integration with the project, offering:
- **MQTT Protocol** for efficient message exchange.
- **Rules Engine** for real-time data processing and device-to-cloud communication.
- **SDK Support** for easy setup and deployment.

## ⚙️ Setup Guide

### Requirements
- STM32U575ZIT6Q microcontroller
- AWS IoT Core account
- Development tools (refer to the [Tools & Documentation](#tools--documentation) section)

### Steps
1. **Connect STM32 with AWS IoT Core**: Configure MQTT and TLS 1.2 for secure communication.
2. **Certificates and Policies**: Apply security certificates for device authentication.
3. **IoT Core Rules Engine**: Set up rules to automate data processing and alerts.

For detailed setup instructions, refer to the `setup.md` file in this repository.

## 📚 Tools & Documentation

- **AWS SDK Documentation**: Access AWS IoT Core SDKs [here](https://aws.amazon.com/sdk-for-iot-core).
- **STM32 Developer Documentation**: STM32 security features and setup guides available [here](https://www.st.com/en/microcontrollers-microprocessors/stm32.html).

## 🎥 Demo

See the project in action! This demo covers:
- **Real-time Data Transmission**: Secure communication between STM32 and AWS IoT Core.
- **Alerts & Notifications**: Triggered based on pre-defined rules in the AWS IoT Core Rules Engine.