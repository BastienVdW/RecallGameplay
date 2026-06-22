# RecallGameplay - Unreal Engine Plugin

This is the **RecallGameplay** plugin, a comprehensive gameplay extension library built on top of the Recall ECS framework, providing advanced gameplay systems, attribute management, simulation rewind, voxel grids, and specialized content for deterministic multiplayer games.

## About RecallGameplay

RecallGameplay is an extensive gameplay framework that extends the Recall ECS system with a complete suite of gameplay mechanics and systems. It offers:

- **Core Gameplay Framework**: Foundational gameplay systems and utilities built on Recall ECS
- **Attribute & Effect Systems**: Comprehensive attribute management with deterministic calculations and effect processing
- **Simulation Rewind**: Ring-buffer based simulation rewind system for rollback support
- **Conversation Integration**: Conversation systems with ECS entity participation
- **Device Management**: Input device and controller management for multiplayer scenarios
- **Game Rule Processing**: Flexible game rule systems for various game modes
- **Navigation Systems**: Advanced pathfinding and navigation for ECS entities
- **Inventory Management**: Complete inventory and item systems
- **Visual Representation**: ECS entity visual representation management
- **Online Integration**: Multiplayer and online gameplay systems
- **Track & Physics**: Track-based entity systems with physics body management
- **Voxel Grid**: Voxel grid system for spatial data representation
- **Grid Selection**: Grid-based cursor, obstacle, and selection systems
- **Editor Tools**: Custom editor tools and asset management

## Plugin Description

*Comprehensive gameplay extension framework for Recall ECS*

This plugin is a **comprehensive gameplay library** built on top of the Recall ECS framework with the following key features:
- **Attribute Systems** for character stats, progression, and effect processing
- **Simulation Rewind** with ring-buffer snapshot storage (up to 512 frames)
- **Conversation Systems** with ECS integration and multiplayer support
- **Device Management** for input handling and controller support
- **Game Rules** for flexible gameplay mode implementation
- **Navigation** and pathfinding systems for ECS entities
- **Inventory** and item management systems
- **Voxel & Grid** spatial systems for world interaction
- **Visual Representation** management for ECS gameplay entities
- **Online Gameplay** integration for multiplayer scenarios

## Origin and Development

**Based on**: Recall ECS Framework  
**Created by**: Bastien Van de Walle  
**Purpose**: Complete gameplay extension framework for Recall  
**Architecture**: Modular gameplay systems with ECS integration  

## Core Modules

### RecallGameplayCore (Runtime)
**Core gameplay framework and utilities**
- **Core Framework**: Fundamental gameplay framework components built on Recall ECS
- **Utility Systems**: Essential utility functions and helpers
- **Data Structures**: Core data structures for gameplay systems
- **Configuration**: Gameplay system configuration and settings
- **Foundation**: Foundation classes for all gameplay modules

### RecallGameplay (Runtime)
**Main gameplay systems and integration**
- **Gameplay Integration**: Primary gameplay logic integrated with Recall ECS
- **MultiWorld Support**: Optional multi-world gameplay integration
- **UI Integration**: Slate and UMG integration for gameplay UI
- **Variable Collection**: Dynamic variable and configuration management
- **System Coordination**: Coordination layer between gameplay modules

### RecallAttributeModule (Runtime)
**Comprehensive attribute and stat management**
- **Attribute System**: Advanced character attributes and statistics
- **Stat Calculations**: Deterministic stat processing and modifiers
- **Progression Systems**: Character progression and leveling mechanics
- **Modifier Framework**: Attribute modifier and effect systems
- **Serialization**: Full attribute state serialization for rollback

### RecallGameplayEffectModule (Runtime)
**Gameplay effect processing**
- **Effect System**: Configurable gameplay effect application
- **Effect Stacking**: Complex effect stacking and interaction rules
- **Duration Management**: Timed and persistent effect handling
- **Effect Triggers**: Condition-based effect activation
- **Modifier Integration**: Direct integration with the attribute modifier system

### RecallRewind (Runtime)
**Simulation rewind and rollback system**
- **Rewind Subsystem**: `URecallRewindSubsystem` for triggering simulation rewinds
- **Ring Buffer Storage**: Up to 512 snapshot frames stored for rollback
- **Snapshot Integration**: Hooks into the Recall snapshot system
- **Simulation Lifecycle**: Implements `IRecallSimulationReactSystemInterface` for clean reset
- **Deterministic Rollback**: Designed for deterministic multiplayer rollback support

### RecallGameRuleModule (Runtime)
**Flexible game rule and mode systems**
- **Rule Engine**: Configurable game rule processing system
- **Game Modes**: Support for multiple game mode implementations
- **Victory Conditions**: Flexible victory and completion condition systems
- **Rule Validation**: Game state validation against active rules
- **Mode Transitions**: Smooth transitions between different game modes

### RecallGameplayRepresentation (Runtime)
**Visual representation for ECS gameplay entities**
- **Representation Management**: Manages visual actors/components for ECS entities
- **Conversation Participation**: `RecallConversationParticipantSubsystem` for entity conversations
- **Function Library**: Blueprint-accessible representation utility functions
- **Editor Integration**: Optional editor framework support at runtime

### RecallGameplayOnline (Runtime)
**Multiplayer and online gameplay integration**
- **Online Systems**: Online session and matchmaking integration
- **State Synchronization**: Gameplay state synchronization across clients
- **Player Management**: Online player tracking and management
- **Network Integration**: Hooks into RecallCore and RecallGameplayCore for networked play

### RecallNavigation (Runtime)
**Advanced navigation and pathfinding**
- **ECS Navigation**: Navigation system designed for Recall ECS entities
- **Pathfinding**: High-performance pathfinding algorithms
- **Dynamic Obstacles**: Real-time obstacle detection and avoidance
- **MultiWorld Support**: Navigation across multiple world instances
- **Movement Coordination**: Coordinated movement for multiple entities

### RecallInventoryModule (Runtime)
**Complete inventory and item management**
- **Inventory System**: Flexible inventory management for ECS entities
- **Item Framework**: Comprehensive item system with properties and behaviors
- **Container Management**: Various container types and storage systems
- **Item Interactions**: Item usage, combination, and interaction systems
- **Persistence**: Inventory state serialization for rollback support

### RecallConversationModule (Runtime)
**Conversation systems for ECS gameplay**
- **ECS Integration**: Native integration with Recall entity systems
- **Multiplayer Support**: Conversation synchronization across clients
- **State Management**: Conversation state tracking and persistence
- **Variable Collection**: Conversation variable integration via VariableCollection
- **Participant Management**: Multi-participant conversation handling

### RecallDeviceModule (Runtime)
**Input device and controller management**
- **Device Detection**: Automatic input device detection and management
- **Controller Support**: Enhanced controller and gamepad support
- **Input Mapping**: Dynamic input mapping and configuration
- **Multiplayer Input**: Multi-device input handling for multiplayer
- **Device State**: Device connection state management

### RecallTrack (Runtime)
**Track-based entity and physics systems**
- **Track Fragments**: `FRecallTrackFragment` for ECS entity track assignment by name
- **Physics Body Parameters**: `FRecallTrackConstSharedFragment` with mesh shape settings
- **Track Subsystem**: `RecallTrackSubsystem` for track lifecycle management
- **Physics Integration**: Jolt physics body management per track entity
- **Track Traits**: ECS traits for automatic track fragment configuration

### RecallVoxel (Runtime)
**Voxel grid system for spatial data**
- **Voxel Grid**: `FRecallVoxelGrid` 3D grid with per-voxel character data
- **Voxel Fragments**: ECS fragments for voxel-associated entities
- **Voxel Traits**: ECS traits for automatic voxel configuration
- **Utility Functions**: Helper functions for voxel grid operations
- **Spatial Representation**: Compact spatial data representation for gameplay use

### RecallGridSelection (Runtime)
**Grid-based cursor and selection systems**
- **Grid Cursor**: `FRecallGridCursorFragment` for player cursor tracking on the grid
- **Grid Obstacles**: `FRecallGridObstacleFragment` for obstacle registration
- **Grid Registry**: `RecallGridRegistry` for grid asset management
- **Selection Subsystem**: `RecallGridSelectionSubsystem` for selection state
- **Function Library**: Blueprint-accessible grid selection utility functions

### RecallEditor (Editor)
**Editor tools and asset management**
- **Asset Editors**: Custom asset editors for RecallGameplay content
- **Python Integration**: Python script plugin support for editor automation
- **MovieScene Integration**: Sequencer and cinematic tool support
- **Slate/UMG Tools**: Custom editor widgets and UI components
- **Workflow Tools**: Development workflow optimization tools

## Key Features

### Simulation Rewind
- **Ring Buffer**: Up to 512 snapshot frames stored in a ring buffer
- **World Subsystem**: `URecallRewindSubsystem` cleanly integrates with the Recall simulation lifecycle
- **Deterministic**: Designed for use with deterministic fixed-timestep ECS simulation
- **Rollback Ready**: Full integration with the Recall snapshot system

### Voxel & Grid Systems
- **Voxel Grid**: 3D voxel grids stored as flat arrays for cache efficiency
- **Grid Selection**: Dedicated cursor and obstacle tracking for grid-based gameplay
- **ECS Native**: Both systems expose Mass fragments and traits for zero-boilerplate setup

### Comprehensive Attribute & Effect Pipeline
- **Flexible Attributes**: Configurable attribute types and calculations
- **Effect Module**: Separate effect processing module layered on top of attributes
- **Modifier Stacking**: Complex modifier chains with proper stacking rules
- **Serialization**: Complete attribute and effect state serialization

## Architecture Overview

### Plugin Module Architecture
```
RecallGameplay Plugin
├── RecallGameplayCore     (Foundation and utilities)
├── RecallGameplay         (Primary gameplay integration)
├── RecallAttributeModule  (Stats and progression)
├── RecallGameplayEffectModule (Effect processing)
├── RecallRewind           (Simulation rollback, ring buffer)
├── RecallGameRuleModule   (Modes and victory conditions)
├── RecallGameplayRepresentation (Visual entity representation)
├── RecallGameplayOnline   (Multiplayer integration)
├── RecallNavigation       (Pathfinding and movement)
├── RecallInventoryModule  (Items and containers)
├── RecallConversationModule (Dialog and narrative)
├── RecallDeviceModule     (Input and controllers)
├── RecallTrack            (Track physics entities)
├── RecallVoxel            (Voxel grid system)
├── RecallGridSelection    (Grid cursor and obstacle systems)
└── RecallEditor           (Development and asset management)
```

### System Integration Flow
```
Game Rules → Attributes → Effects → Inventory → UI Representation
     ↓           ↓           ↓         ↓              ↓
Navigation → Animation → Interaction → Conversation → Online Sync
     ↓
Rewind ← Snapshot ← ECS Simulation (fixed timestep, deterministic)
```

### ECS Integration
```
Recall ECS → RecallGameplay Extensions → Game Logic → Visual Representation
```

## Integration

The plugin integrates with:
- **Recall**: Core ECS framework with rollback and snapshot support (Required)
- **Mass Entity**: Unreal Engine Mass ECS used directly with engine-level patches (no duplicate/fork layer)
- **VariableCollection**: Dynamic variable and configuration management (Required)
- **ExtendedCommonUI**: Enhanced UI framework for gameplay (Required)
- **CommonConversation**: Conversation system integration (Required)
- **StateTree**: AI and behavior tree integration
- **Enhanced Input**: Modern input system integration
- **Niagara**: Particle systems integration
- **Game Features**: Modular game feature integration

### Mass Integration Approach

Unlike earlier iterations that relied on a forked Mass layer (MassExtended), RecallGameplay integrates directly with Unreal Engine's Mass ECS and applies targeted patches at the engine level. This keeps the codebase closer to engine source, reduces maintenance overhead, and makes engine upgrades more straightforward.

## Configuration

### Basic Setup
1. **Enable Plugin**: Enable RecallGameplay in project plugins
2. **Configure Dependencies**: Ensure Recall, VariableCollection, ExtendedCommonUI, and other dependencies are enabled
3. **Setup Modules**: Configure required gameplay modules for your project
4. **Initialize Systems**: Set up gameplay systems and configurations

### Gameplay Configuration
1. **Attribute Setup**: Configure attribute types and progression systems
2. **Effect Setup**: Configure gameplay effects and modifier stacks
3. **Conversation Setup**: Set up conversation trees and dialog systems
4. **Game Rules**: Define game modes and victory conditions
5. **Grid Setup**: Configure voxel grids and grid selection registries

## Key Differences from Standard Gameplay Frameworks

### ECS Integration
- **Standard Frameworks**: Actor-based gameplay systems
- **RecallGameplay**: Native ECS entity-based systems via Recall

### Deterministic Design
- **Standard Frameworks**: Variable timestep, non-deterministic
- **RecallGameplay**: Fixed timestep, fully deterministic for multiplayer

### Rollback Support
- **Standard Frameworks**: Limited state management
- **RecallGameplay**: Ring-buffer snapshot system with `URecallRewindSubsystem` for full rollback

### Spatial Systems
- **Standard Frameworks**: Actor-based spatial queries
- **RecallGameplay**: ECS-native voxel and grid selection systems

### Mass Usage
- **Previous (MassExtended)**: Forked/duplicated Mass layer with custom modifications
- **RecallGameplay**: Integrates directly with engine Mass ECS via targeted patches — no fork to maintain

## Developer Notes

- **Author**: Bastien Van de Walle
- **Category**: Gameplay
- **Version**: 1.0
- **Default State**: Disabled by default (opt-in per project)
- **Content Support**: Enabled for gameplay assets and templates
- **Framework**: Built on Recall ECS with comprehensive gameplay extensions
- **License**: Apache License 2.0