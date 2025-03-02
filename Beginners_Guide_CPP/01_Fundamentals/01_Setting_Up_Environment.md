# Setting Up Your Development Environment

## Overview

Before you can start developing C++ games in Unreal Engine 5, you need to set up your development environment properly. This guide will walk you through the essential steps to get everything configured correctly.

## Required Software

You'll need to install the following software:

1. **Unreal Engine 5** - The game engine we'll be using
2. **Visual Studio 2022** (Windows) or **Xcode** (Mac) - The IDE for C++ development
3. **Cursor IDE** - Our recommended IDE for its AI-assisted coding capabilities
4. **Git** - For version control

## Step-by-Step Installation Guide

### 1. Installing Unreal Engine 5

1. Download and install the Epic Games Launcher from [Epic Games website](https://www.epicgames.com/store/en-US/download)
2. Create an Epic Games account or sign in
3. In the launcher, navigate to the "Unreal Engine" tab
4. Click "Install Engine" and select version 5.x (latest stable release)
5. Choose the installation location (requires ~100GB of free space)
6. Select the following components:
   - Engine
   - Templates and feature packs
   - Engine source code (required for C++ development)
7. Click "Install" and wait for the process to complete

#### Verification
Once installed, launch the Epic Games Launcher, go to the Library tab, and verify you can launch Unreal Engine 5.

### 2. Installing the Code Editor

#### For Windows: Visual Studio 2022
1. Download Visual Studio 2022 Community Edition from [Microsoft's website](https://visualstudio.microsoft.com/)
2. During installation, select the following workloads:
   - Game development with C++
   - Desktop development with C++
3. In the Individual components tab, ensure these are selected:
   - C++ profiling tools
   - C++ AddressSanitizer
   - Windows 10/11 SDK
4. Complete the installation

#### For Mac: Xcode
1. Open the App Store
2. Search for "Xcode" and install it
3. Open Xcode after installation and accept the license agreement
4. Install additional components when prompted

### 3. Installing Cursor IDE

1. Download Cursor IDE from [Cursor's website](https://cursor.sh/)
2. Follow the standard installation process for your operating system
3. Launch Cursor and follow the setup wizard
4. Configure your preferences for AI assistance
5. (Optional) Connect your GitHub account for easier repository management

### 4. Installing Git

1. Download Git from [git-scm.com](https://git-scm.com/downloads)
2. During installation, use the recommended settings
3. After installation, open a terminal/command prompt and verify by typing:
   ```
   git --version
   ```

## Configuring Your Development Environment

### Setting Up Unreal Engine with Your Code Editor

1. Launch the Epic Games Launcher
2. Go to Unreal Engine > Settings
3. Under "General", find the "Editors" section
4. Set your primary code editor to Visual Studio/Xcode
5. Click "Accept"

### Creating Your First C++ Project

1. Launch Unreal Engine from the Epic Games Launcher
2. Select "New Project"
3. Choose "Games" as the category
4. Select "Third Person" template
5. Ensure "C++" is selected (not Blueprint)
6. Choose your project location
7. Click "Create Project"

The engine will generate the necessary C++ files and open your code editor.

### Opening the Project in Cursor IDE

1. Once your project is created and the initial build is complete
2. Launch Cursor IDE
3. Choose "Open Folder" and navigate to your project folder
4. Select the folder and click "Open"

## Testing Your Setup

To make sure everything is set up correctly:

1. In Unreal Editor, go to File > Open C++ Class
2. Select "ThirdPersonCharacter" (or similar based on your template)
3. The class should open in your code editor
4. Add a simple log message to the BeginPlay function:
   ```cpp
   void AYourCharacter::BeginPlay()
   {
       Super::BeginPlay();
       
       UE_LOG(LogTemp, Warning, TEXT("Hello World from C++!"));
   }
   ```
5. Save the file
6. Return to Unreal Editor and click the "Compile" button
7. Run the game in the editor
8. Check the Output Log (Window > Developer Tools > Output Log)
9. You should see your "Hello World" message

## Common Issues and Solutions

### Compile Errors on First Build

**Problem**: First build fails with numerous errors.  
**Solution**: Make sure all Visual Studio/Xcode components are properly installed. Restart your computer and try again.

### Missing Unreal Header Tool

**Problem**: Error about missing UnrealHeaderTool.  
**Solution**: Ensure you installed the Engine source code during UE5 installation.

## Next Steps

Now that your development environment is set up, you're ready to start learning C++ for Unreal Engine 5. Continue to the next guide: [C++ Basics for Unreal Engine](02_CPP_Basics_For_UE5.md).
