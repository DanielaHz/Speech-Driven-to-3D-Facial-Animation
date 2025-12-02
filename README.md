# Speech-Drive 3D Facial Animation

This project delivers a plugin that integrates a speech‑driven AI model into animation pipelines, enabling the generation of facial animations from portraits and audio inputs. It is still a prototype, but the idea behind it is to help animators create expressive facial motion based on video generation while maintaining full control over the animation data directly within Maya.

## Demo

![image](./assets/Demo.gif)

Complete link Demo: https://youtu.be/t6xBt4I6aKw

## Paper Based on Application 
This application is based on the paper named as: A Facial Motion Retargeting Pipeline for Appearance
Agnostic 3D Characters (2024)

## Features
- **Facial animation from portraits and audio**
- **Maya plugin integration**

## Facial Landmarks System

![image](./assets/Facial-landmarks.png)

The system we use is called **Dense Facial Landmarks**. Its goal is to **track facial geometry based on the density of key regions**, focusing on how facial muscles intersect and influence expression. The model uses a total of **51 landmark points** to represent critical areas of the face.

## Topology-Based Muscle-Rig

| Element | Description |
| --- | --- |
| Vertices | 6k |
| Uniform Topology | Ensures the rig works consistently across all meshes |
| Symmetry | Allows mirrored deformations and symmetrical muscle behavior. |
| Vertex Order| Enables automated muscle assignments based on consistent indexing |
| Single mesh | Facilitates continuous deformations and simplifies rigging.|

## Muscles regions

![image](./assets/MuscleModelReference.jpg)

## System Design

### Workflow

![image](./assets/WorkflowSystem.png)

### UML
![image](./assets/classDiagram.png)

### Landmarks workflow

![image](./assets/landmarksworkflow.png)

### Source Motion Interpretation
![image](./assets/SourceMotionInterpretation.png)

### Target Face Parametrization

![image](./assets/Tergatfaceparametrization.png)

### Motion Retargeting

![image](./assets/motionretargeting.png)

## Prerequisites (Linux)
- [Maya Devkit minimum GCC 11.2.1](https://aps.autodesk.com/developer/overview/maya)
- C++17 

***Note: Important information about the Devkit!!!!: after downloading and decompressing it, you need to:***
```
cd Devkit/include
tar -xzf qt_5.15.2-include.tar.gz
```
***Otherwise, you may encounter issues with Qt and some Maya libraries.***

## Requirements

1. run the next command:
```
cd cmd/retargeting
./requirements.sh
```

2. run the next command:
```
cd ../../
vcpkg install 
```

## Build Instructions

```
./build.sh
```

## Timeline
Link: https://docs.google.com/spreadsheets/d/1x9G0XLGNLBWYCOzbNsvGBKxYSU4ziLk73EwxFMBnzFg/edit?usp=sharing

## Project Management
Link: https://bournemouth-team-nk4tu6dm.atlassian.net/jira/software/projects/MAS/boards/35?atlOrigin=eyJpIjoiYzI2YjA3MzA2YzAwNGE3YmJjNzZiMjI5ODcxMTE0MGQiLCJwIjoiaiJ9

## Useful Links
-[ Maya SDK/Devkit Setup Linux](https://help.autodesk.com/view/MAYADEV/2026/ENU/?guid=Maya_DEVHELP_Setting_up_your_build_Linux_environment_html)
- https://help.autodesk.com/view/MAYAUL/2026/ENU/?guid=GUID-83C9793D-54AB-4BA3-812B-005D8153A79C
- https://download.autodesk.com/us/maya/2010help/api/class_m_fn_skin_cluster.html
- https://imotions.com/blog/learning/research-fundamentals/facial-action-coding-system/
- https://imotions.com/blog/learning/research-fundamentals/facial-action-coding-system/?srsltid=AfmBOop8kRPovAn-gvcuY2s7Cl1c7AchXmI4CL_3zS-ud2xIMwRWc_vU
