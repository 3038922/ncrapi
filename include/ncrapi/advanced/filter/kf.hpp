#pragma once
// #include "ncrapi//Eigen/Core"
// #include "ncrapi//Eigen/Dense"
// #include "ncrapi//Eigen/Eigen"
// #include "ncrapi//Eigen/LU"

// namespace ncrapi {
// template <size_t _nums>
// class Kf
// {
//   private:
//     Eigen::Matrix<double, _nums, 1> X_last = Eigen::Matrix<double, _nums, 1>::Zero();         //x(k-1|k-1)
//     Eigen::Matrix<double, _nums, 1> X_pre;                                                    //x(k|k-1)
//     Eigen::Matrix<double, _nums, 1> X_now;                                                    //x(k|k)
//     Eigen::Matrix<double, _nums, _nums> P_last = Eigen::Matrix<double, _nums, _nums>::Zero(); //p(k-1|k-1)getrow
//     Eigen::Matrix<double, _nums, _nums> P_pre;                                                //p(k|k-1)
//     Eigen::Matrix<double, _nums, _nums> P_update;                                             //p(k|k)
//     Eigen::Matrix<double, _nums, _nums> Kg;                                                   //kg(k)
//     Eigen::Matrix<double, _nums, _nums> trans_A;                                              //matrix_A
//     Eigen::Matrix<double, _nums, _nums> meas_H;                                               //mattix_H
//   protected:
//     Eigen::Matrix<double, _nums, 1> measure_Z;
//     double Noise_Q; //Systerm noise
//     double Noise_R; //Measurement noise
//   public:
//     Kf(const double ProcessNoise_Q, const double MeasureNoise_R) : Noise_Q(ProcessNoise_Q), Noise_R(MeasureNoise_R)
//     {
//         int count = 0;
//         for (int i = 0; i < _nums; i++)
//             for (int j = 0; j < _nums; j++)
//             {
//                 if (i == j)
//                 {
//                     trans_A(count) = 1;
//                     meas_H(count) = 1;
//                 }
//                 else
//                 {
//                     trans_A(count) = 0;
//                     meas_H(count) = 0;
//                 }
//                 count++;
//             }
//     }
//     ~Kf() {}
//     Eigen::Matrix<double, _nums, 1> filter(Eigen::Matrix<double, _nums, 1> Data)
//     {
//         measure_Z = Data;
//         X_pre = trans_A * X_last;
//         P_pre = trans_A * P_last * trans_A.transpose() + Noise_Q * trans_A;
//         Eigen::Matrix<double, _nums, _nums> temp = (meas_H * P_pre * meas_H.transpose() + Noise_R * meas_H);
//         Kg = P_pre * meas_H.transpose() * temp.inverse();
//         X_now = X_pre + Kg * (measure_Z - meas_H * X_pre);
//         P_update = (trans_A - Kg) * P_pre;

//         X_last = X_now;
//         P_last = P_update;
//         return X_now;
//     }
// };

// } // namespace ncrapi