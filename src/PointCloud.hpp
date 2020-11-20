#ifndef NAMINUKAS_BRAIN_POINTCLOUD_H_
#define NAMINUKAS_BRAIN_POINTCLOUD_H_

class PointCloud {
  public:
    PointCloud(std::function<bool()> is_terminated);
};

#endif  // NAMINUKAS_BRAIN_POINTCLOUD_H_
