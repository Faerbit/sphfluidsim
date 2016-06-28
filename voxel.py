def voxel(voxel_id):
    length = 4
    z = voxel_id // (length*length)
    voxel_id = voxel_id %(length*length)
    y = voxel_id // length
    x = voxel_id % length
    return x, y, z
