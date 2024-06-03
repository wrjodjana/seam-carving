#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}

const ImagePPM& SeamCarver::GetImage() const { return image_; }

int SeamCarver::GetHeight() const { return height_; }

int SeamCarver::GetWidth() const { return width_; }

int SeamCarver::GetEnergy(int row, int col) const {
  int left_col = 0;
  int right_col = 0;
  int top_row = 0;
  int bottom_row = 0;
  if (col == 0) {
    left_col = width_ - 1;
  } else {
    left_col = col - 1;
  }
  if (col == width_ - 1) {
    right_col = 0;
  } else {
    right_col = col + 1;
  }
  if (row == 0) {
    top_row = height_ - 1;
  } else {
    top_row = row - 1;
  }
  if (row == height_ - 1) {
    bottom_row = 0;
  } else {
    bottom_row = row + 1;
  }
  int red_col = image_.GetPixel(row, left_col).GetRed() -image_.GetPixel(row, right_col).GetRed();
  int green_col = image_.GetPixel(row,left_col).GetGreen() -image_.GetPixel(row, right_col).GetGreen();
  int blue_col = image_.GetPixel(row, left_col).GetBlue() -image_.GetPixel(row, right_col).GetBlue();
  int red_row = image_.GetPixel(top_row, col).GetRed() -image_.GetPixel(bottom_row, col).GetRed();
  int green_row = image_.GetPixel(top_row, col).GetGreen() -image_.GetPixel(bottom_row, col).GetGreen();
  int blue_row = image_.GetPixel(top_row, col).GetBlue() - image_.GetPixel(bottom_row, col).GetBlue();
  int delta_squared_col =
      (red_col * red_col) + (green_col * green_col) + (blue_col * blue_col);
  int delta_squared_row =
      (red_row * red_row) + (green_row * green_row) + (blue_row * blue_row);
  int energy = delta_squared_col + delta_squared_row;
  return energy;
}

int** SeamCarver::FindMinPathVertical() const {
  int** values = new int*[height_];
  for (int row = 0; row < height_; row++) {
    values[row] = new int[width_];
  }

  for (int col = 0; col < width_; col++) {
    values[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }

  for (int row = height_ - 2; row >= 0; row--) {
    for (int col = 0; col < width_; col++) {
      int left = kMax;
      int down = values[row + 1][col];
      int right = kMax;

      if (col > 0) {
        left = values[row+1][col-1];
      }

      if (col < width_ - 1) {
        right = values[row+1][col+1];
      }

      int min_path = MinPathTriple(left, down, right);
      values[row][col] = GetEnergy(row, col) + min_path;
    }
  }
  return values;
}

int** SeamCarver::FindMinPathHorizontal() const {
  int** values = new int*[height_];
  for (int row = 0; row < height_; row++) {
    values[row] = new int[width_];
  }
  for (int row = 0; row < height_; row++) {
    values[row][width_ - 1] = GetEnergy(row, width_ - 1);
  }
  for (int col = width_ -2; col >= 0; col--) {
    for (int row = 0; row < height_; row++) {
      int up = kMax;
      int right =  values[row][col+1];
      int down = kMax;

      if (row > 0) {
        up = values[row-1][col+1];
      }

      if (row < height_ -1) {
        down = values[row+1][col+1];
      }

      int min_path = MinPathTriple(right, up, down);
      values[row][col] = GetEnergy(row, col) + min_path;
    }
  }
  return values;
}

int SeamCarver::FindMinVertIndex(const int* arr, int arr_length) const {
  int min_index = 0;
  for (int row = 1; row < arr_length; row++) {
    if (arr[row] < arr[min_index]) {
      min_index = row;
    }
  }
  return min_index;
}

int SeamCarver::FindMinHorizIndex(int** values, int arr_length) const {
  int min_index = 0;
  for (int i = 1; i < arr_length; i++) {
    if (values[i][0] <values[min_index][0]) {
      min_index = i;
    }
  }
  return min_index;
}

int SeamCarver::GetNextColumn(int** values, int row, int col) const {
    if (col == 0) {
        if (values[row][col] <= values[row][col + 1]) {
            return col;
        } else {
            return col + 1;
        }
    }
    else if (col == width_ - 1) {
        if (values[row][col] <= values[row][col - 1]) {
            return col;
        } else {
            return col - 1;
        }
    }
    else {
        int min_col = col;
        if (values[row][col - 1] < values[row][min_col]) {
            min_col = col - 1;
        }
        if (values[row][col + 1] < values[row][min_col]) {
            min_col = col + 1;
        }
        return min_col;
    }
}


int SeamCarver::GetNextRow(int** values, int col, int row) const {
  if (row == 0) {
    if (values[row + 1][col] < values[row][col]) {
      return row + 1;
    } else {
      return row;
    }
  } else if (row == height_ - 1) {
    if (values[row - 1][col] < values[row][col]) {
      return row - 1;
    } else {
      return row;
    }
  } else {
    int min_row = row;
    if (values[row - 1][col] < values[min_row][col]) {
      min_row = row - 1;
    }
    if (values[row + 1][col] < values[min_row][col]) {
      min_row = row + 1;
    }
    return min_row;
  }
}

int* SeamCarver::GetHorizontalSeam() const {
  int* seam = new int[width_];
  int** values = FindMinPathHorizontal();

  int min_row = FindMinHorizIndex(values,height_);
  seam[0] = min_row;

  for (int col = 1; col < width_; col++) {
    min_row = GetNextRow(values, col, min_row);
    seam[col] = min_row;
  }

  for (int row = 0; row < height_; row++) {
    delete[] values[row];
  }
  delete[] values;
  values = nullptr;
  return seam;
}

int* SeamCarver::GetVerticalSeam() const {
  int* seam = new int[height_];
  int** values = FindMinPathVertical();

  int min_col = FindMinVertIndex(values[0], width_);
  seam[0] = min_col;

  for (int row = 1; row < height_; row++) {
    min_col = GetNextColumn(values, row, min_col);
    seam[row] = min_col;
  }

  for (int row = 0; row < height_; row++) {
    delete[] values[row];
  }
  delete[] values;
  values = nullptr;
  return seam;

}


void SeamCarver::RemoveHorizontalSeam() {
  int* seam = GetHorizontalSeam();
  Pixel** pixels = new Pixel*[height_ - 1];

  for (int row = 0; row < height_ - 1; ++row) {
    pixels[row] = new Pixel[width_];
  }

  for (int col = 0; col < width_; ++col) {
    int seam_row = seam[col];
    int temp_row = 0;
    for (int row = 0; row < height_; ++row) {
      if (row == seam_row) {
        continue;
      }
      pixels[temp_row++][col] = image_.GetPixel(row,col);
    }
  }
  image_.Pixels(pixels, height_ - 1, width_);
  height_ = height_ - 1;
  delete[] seam;
}

void SeamCarver::RemoveVerticalSeam() {
  int* seam = GetVerticalSeam();
  Pixel** pixels = new Pixel*[height_];

  for (int row = 0; row < height_; ++row) {
    pixels[row] = new Pixel[width_ -1];
  }

  for (int row = 0; row < height_; ++row) {
    int seam_col = seam[row];
    int temp_col = 0;
    for (int col = 0; col < width_; ++col) {
      if (col == seam_col) {
        continue;
      }
      pixels[row][temp_col++] = image_.GetPixel(row,col);
    }
  }
  image_.Pixels(pixels, height_, width_ - 1);
  width_ = width_ - 1;
  delete[] seam;
}


int SeamCarver::MinPathTriple(int value1, int value2, int value3) const {
  int min = value1;
  if (value2 < min) {
    min = value2;
  }
  if (value3 < min) {
    min = value3;
  }
  return min;
}