#include <opencv2/imgproc/imgproc.hpp>

#include "AffineTransformation.hpp"

// Constructors.
AffineTransformation::AffineTransformation(
  double angle,
  cv::Point center,
  double scaleX,
  double scaleY,
  double skewFactorX,
  double skewFactorY) {
  initializeTransformation(angle, center, scaleX, scaleY, skewFactorX, skewFactorY);
}

AffineTransformation::AffineTransformation(const cv::Mat& transformation, bool shouldBeInverted) {
  if (shouldBeInverted) {
    initiateByInversedTransformMatrix(transformation);
  } else {
    transformationMatrix = transformation.clone();
  }
}

// Private methods.
void AffineTransformation::initializeTransformation(
                              double angle,
                              cv::Point center,
                              double scaleX,
                              double scaleY,
                              double skewFactorX,
                              double skewFactorY) {
  cv::Point2f centerPoint;
  centerPoint.x = static_cast<float>(center.x);
  centerPoint.y = static_cast<float>(center.y);

  cv::Mat rotationMatrix = cv::getRotationMatrix2D(centerPoint, angle, 1.0);

  double scaleMatrixPoints[] = { scaleX, 0, 0, scaleY };
  cv::Mat scaleMatrix = cv::Mat(2, 2, CV_64FC1, scaleMatrixPoints);

  double skewPoints[] = { 1, skewFactorX, skewFactorY, 1 };
  cv::Mat skewMatrix = cv::Mat(2, 2, CV_64FC1, skewPoints);

  cv::Mat tempMatrix = scaleMatrix * rotationMatrix;
  transformationMatrix = skewMatrix * tempMatrix;
}

void AffineTransformation::initiateByInversedTransformMatrix(const cv::Mat& transformation) {
  double matrixPoints[] = {
    transformation.at<double>(0, 0),
    transformation.at<double>(0, 1),
    transformation.at<double>(1, 0),
    transformation.at<double>(1, 1)
  };

  cv::Mat matrix = cv::Mat(2, 2, CV_64FC1, matrixPoints);

  double negativeMovePoints[] = {
    -transformation.at<double>(0, 2),
    -transformation.at<double>(1, 2)
  };

  cv::Mat negativeMoveVector = cv::Mat(2, 1, CV_64FC1, negativeMovePoints);
  cv::Mat inversedMatrix = matrix.inv();
  cv::Mat inversedMoveVector = inversedMatrix * negativeMoveVector;

  transformationMatrix = cv::Mat(2, 3, CV_64FC1);

  transformationMatrix.at<double>(0, 0) = inversedMatrix.at<double>(0, 0);
  transformationMatrix.at<double>(0, 1) = inversedMatrix.at<double>(0, 1);
  transformationMatrix.at<double>(1, 0) = inversedMatrix.at<double>(1, 0);
  transformationMatrix.at<double>(1, 1) = inversedMatrix.at<double>(1, 1);
  transformationMatrix.at<double>(0, 2) = inversedMoveVector.at<double>(0);
  transformationMatrix.at<double>(1, 2) = inversedMoveVector.at<double>(1);
}

cv::Point2d AffineTransformation::multiplyMatrixWithVector(const cv::Point2d& vector) const {
  double coordinates[] = { vector.x, vector.y, 1 };
  cv::Mat pointVector = cv::Mat(3, 1, CV_64FC1, coordinates);

  double resultPoint[] = { 0, 0 };
  cv::Mat resultVector = cv::Mat(2, 1, CV_64FC1, resultPoint);
  resultVector = transformationMatrix * pointVector;

  cv::Point2d result;
  result.x = resultVector.at<double>(0, 0);
  result.y = resultVector.at<double>(1, 0);

  return result;
}

// Public methods.
cv::Point AffineTransformation::transformPoint(const cv::Point& point) const {
  cv::Point2d initialPoint;
  initialPoint.x = point.x;
  initialPoint.y = point.y;

  cv::Point2d resultPoint = multiplyMatrixWithVector(initialPoint);

  CvPoint toReturn;
  toReturn.x = static_cast<int>(resultPoint.x);
  toReturn.y = static_cast<int>(resultPoint.y);

  return toReturn;
}

void AffineTransformation::transformImage(
                              const cv::Mat& inputImage,
                              cv::Mat* transformedImage,
                              cv::Mat* transformMatrix)
{
  cv::Point2d leftTop;
  leftTop.x = 0;
  leftTop.y = 0;
  cv::Point2d leftTopImage = multiplyMatrixWithVector(leftTop);

  cv::Point2d rightTop;
  rightTop.x = inputImage.rows - 1;
  rightTop.y = 0;
  cv::Point2d rightTopImage = multiplyMatrixWithVector(rightTop);

  cv::Point2d leftBottom;
  leftBottom.x = 0;
  leftBottom.y = inputImage.cols - 1;
  cv::Point2d leftBottomImage = multiplyMatrixWithVector(leftBottom);

  cv::Point2d rightBottom;
  rightBottom.x = inputImage.rows - 1;
  rightBottom.y = inputImage.cols - 1;
  cv::Point2d rightBottomImage = multiplyMatrixWithVector(rightBottom);

  cv::Point2d maxPoint;
  maxPoint.x = std::max(leftTopImage.x,
                        std::max(rightTopImage.x,
                                 std::max(leftBottomImage.x, rightBottomImage.x)));
  maxPoint.y = std::max(leftTopImage.y,
                        std::max(rightTopImage.y,
                                 std::max(leftBottomImage.y, rightBottomImage.y)));

  cv::Point2d minPoint;
  minPoint.x = std::min(leftTopImage.x,
                        std::min(rightTopImage.x,
                                 std::min(leftBottomImage.x, rightBottomImage.x)));
  minPoint.y = std::min(leftTopImage.y,
                        std::min(rightTopImage.y,
                                 std::min(leftBottomImage.y, rightBottomImage.y)));

  cv::Size transformedImageSize;
  transformedImageSize.width = static_cast<int>(std::ceil(maxPoint.x - minPoint.x));
  transformedImageSize.height = static_cast<int>(std::ceil(maxPoint.y - minPoint.y));

  transformMatrix = new cv::Mat(2, 3, CV_64FC1);
  transformedImage = new cv::Mat(inputImage.size(), inputImage.depth(), inputImage.channels());

  transformMatrix->at<double>(0, 0) = transformationMatrix.at<double>(0, 0);
  transformMatrix->at<double>(0, 1) = transformationMatrix.at<double>(0, 1);
  transformMatrix->at<double>(0, 2) = transformationMatrix.at<double>(0, 2) - minPoint.x;
  transformMatrix->at<double>(1, 0) = transformationMatrix.at<double>(1, 0);
  transformMatrix->at<double>(1, 1) = transformationMatrix.at<double>(1, 1);
  transformMatrix->at<double>(1, 2) = transformationMatrix.at<double>(1, 2) - minPoint.y;

  cv::warpAffine(inputImage, *transformedImage, *transformMatrix, inputImage.size());
}

void AffineTransformation::transformImage(
                              const cv::Mat& inputImage,
                              cv::Mat& transformedImage,
                              cv::Size boundary) const {
  cv::warpAffine(inputImage, transformedImage, transformationMatrix, inputImage.size());
}