#ifndef MAYAMESH_H_
#define MAYAMESH_H_

#include <maya/MGlobal.h>
#include <maya/MStringArray.h>
#include <maya/MSelectionList.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MStatus.h>
#include <QString>
#include <maya/MString.h>
#include <maya/MFnSkinCluster.h>
#include <glm/vec3.hpp>
#include <vector>
#include <maya/MObjectArray.h>
#include <maya/MFnIkJoint.h>
#include <maya/MItGeometry.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MDoubleArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <optional>
#include <unordered_map>
#include <DCCInterface.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MTypes.h> 
#include <maya/MFloatPointArray.h>   
#include <maya/MFloatVector.h>

/**
 * @class MayaMesh
 * @brief Handles mesh operations within the Maya viewport for the PixelMux plugin.
 *
 * This class is responsible for importing OBJ files, renaming and positioning mesh objects,
 * and placing anatomical reference meshes such as the skull. It supports automatic alignment
 * of skin and muscle meshes to facilitate proximity wrap deformation workflows.
 */
class MayaMesh {
public:
    /**
     * @brief Converts a Qt QString path to Maya's MString format.
     * @param path The input file path from the UI.
     * @return Converted MString path.
     */
    MString convertModelPathToMString(const QString& path);

    /**
     * @brief Renames a mesh transform node in the Maya scene.
     * @param transform The transform node to rename.
     * @param newName The new name to assign.
     * @return Maya status indicating success or failure.
     */
    MStatus renameMesh(const MObject& transform, const MString& newName);

    /**
     * @brief Positions a transform node in the Maya scene.
     * Used to align skin, muscle, and skull meshes.
     * @param transform The transform node to move.
     * @param position The target position vector.
     * @param space The coordinate space (e.g., world or object).
     * @return Maya status indicating success or failure.
     */
    MStatus positionTransform(const MObject& transform, const MVector& position, MSpace::Space space);

    /**
     * @brief Loads a Maya muscle object from the given file path.
     * @param objPath Path to the object file (e.g., .obj or .fbx) to load the muscle.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus loadMayaMuscle(const MString& objPath);

    /**
     * @brief Loads a Maya skin object from the given file path.
     * @param objPath Path to the object file (e.g., .obj or .fbx) to load the skin.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus loadMayaSkin(const MString& objPath);

    /**
     * @brief Loads a Maya skull object from the given file path and scales it based on mesh size.
     * 
     * This method assumes that the skull model needs to be resized according to its dimensions
     * relative to the mesh.
     * 
     * @param objPath Path to the object file (e.g., .obj or .fbx) to load the skull.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus loadMayaSkull(const MString& objPath); // This should scale based on the size of the mesh.

    /**
     * @brief Retrieves the Maya muscle object loaded in memory.
     * @return The loaded MObject representing the muscle.
     */
    MObject getMayaMuscle();

    /**
     * @brief Creates a joint at the specified position with the given name.
     * @param position The 3D position in space where the joint should be created.
     * @param name The name of the joint.
     * @return An MObject representing the created joint.
     */
    MObject createJoint(const MVector& position, const std::string& name);

    /**
     * @brief Creates a skin cluster for the given mesh using the specified joints.
     * 
     * A skin cluster binds the mesh to the joints, enabling deformation based on joint movement.
     * 
     * @param joints An array of MObject instances representing the joints.
     * @param skinMesh The MObject representing the skin mesh to bind to the joints.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus createSkinCluster(const MObjectArray& joints, const MObject& skinMesh);

    /**
     * @brief Prepares the mesh for skinning based on 3D landmark positions.
     * 
     * This method prepares the input mesh for the skinning process by providing the 3D landmarks.
     * 
     * @param m_inputMeshLandmarks3D A vector of glm::vec3 objects representing the 3D landmarks of the mesh.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus prepareMeshSkinning(std::vector<glm::vec3> m_inputMeshLandmarks3D);

    /**
     * @brief Deforms the muscle mesh based on the specified Action Unit deltas and active Action Units.
     * 
     * This method applies muscle deformations based on the AU deltas, modifying the muscle shape
     * based on facial expressions and the distance data from the activated AUs.
     * 
     * @param auDeltaTable A table mapping AU IDs to their corresponding ActionUnitDelta vectors.
     * @param activeAU_opt An optional landmarksDistanceData object representing the active AU, if available.
     * @return MStatus representing the success or failure of the operation.
     */
    MStatus muscleDeformation(const std::unordered_map<int, std::vector<ActionUnitDelta>>& auDeltaTable, const std::optional<landmarksDistanceData>& activeAU_opt);

    /**
     * @brief Imports an OBJ mesh and retrieves its transform and shape nodes.
     * @param objPath Path to the OBJ file.
     * @param transformOut Output reference to the transform node.
     * @param shapeOut Output reference to the shape node.
     * @return Maya status indicating success or failure.
     */
    MStatus importObjMesh(const MString& objPath, MObject& transformOut, MObject& shapeOut, std::string& nameOut);

    /**
     * @brief Places a default skull mesh beneath the muscle mesh.
     * This helps constrain deformation and maintain anatomical accuracy.
     * @return Maya status indicating success or failure.
     */

    MObject findMeshByName(const MString& meshName, MStatus* outStatus) const
    {
        MSelectionList sel;
        MStatus        s = sel.add(meshName);
        if (outStatus) *outStatus = s;
        if (s != MS::kSuccess) return MObject::kNullObj;

        MObject node;
        sel.getDependNode(0, node);
        return node;
    }

private:
    MObject _skinTransform{ MObject::kNullObj };
    MObject _skinShape{ MObject::kNullObj };

    MObject _muscleTransform{ MObject::kNullObj };
    MObject _muscleShape{ MObject::kNullObj };

    MObject _skullTransform{ MObject::kNullObj };
    MObject _skullShape{ MObject::kNullObj };

    MVectorArray _prevAccums;   // stores last frame's accumulations
};

#endif