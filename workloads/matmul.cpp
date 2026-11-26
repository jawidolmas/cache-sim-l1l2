#include<iostream>
#include<vector>
#include<string>



int main(int argc, char* argv[]){
  if(argc != 3){
    std::cout << "The number of words is not 3. Usage: ./matmul ijk/ikj N\n";
    return 1;
  }
  std::string order = argv[1];
  if(order != "ijk" && order != "ikj"){
    std::cout << "Undefined order. Defined: ijk & ikj\n";
    return 1;
  }
  // Take the size of matrix n from user
  int n =std::stoi(argv[2]);
  if(n <= 0) {
    std::cout << "N cannot be smaller or equal to zero\n";
    return 1;
  }
  std::vector<std::vector<double>> A(n, std::vector<double>(n));
  std::vector<std::vector<double>> B(n, std::vector<double>(n));
  int value = 1;
  for(int i = 0; i<n; i++){
    for(int j = 0; j<n; j++){
      A[i][j] = value++;
      B[i][j] = value++;
    }
  }
  std::vector<std::vector<double>> C(n, std::vector<double>(n, 0));
  
  // If order is ijk then multiply in ijk order
  if (order == "ijk") {
      for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
              for (int k = 0; k < n; ++k) {
                  C[i][j] += A[i][k] * B[k][j];
              }
          }
      }
  }else{
    for(int i = 0; i<n; i++){
      for(int k = 0; k<n; k++){
        for(int j = 0; j<n; j++){
          C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
  }

  // Print C[0]
  std::cout << C[0][0] << std::endl;
  return 0;
}