// Copyright 2018 Xiaomi, Inc.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if defined(MACE_ENABLE_NEON)
#include <arm_neon.h>
#endif

#include "mace/kernels/arm/conv_2d_neon.h"

namespace mace {
namespace kernels {

#define Conv2dNeonK7x7SnLoadCalc4                                \
  /* load filter (4 outch x 1 height x 4 width) */               \
  float32x4_t vf00, vf01;                                        \
  float32x4_t vf10, vf11;                                        \
  float32x4_t vf20, vf21;                                        \
  float32x4_t vf30, vf31;                                        \
  vf00 = vld1q_f32(filter_ptr0);                                 \
  vf01 = vld1q_f32(filter_ptr0 + 4);                             \
  vf10 = vld1q_f32(filter_ptr1);                                 \
  vf11 = vld1q_f32(filter_ptr1 + 4);                             \
  vf20 = vld1q_f32(filter_ptr2);                                 \
  vf21 = vld1q_f32(filter_ptr2 + 4);                             \
  vf30 = vld1q_f32(filter_ptr3);                                 \
  vf31 = vld1q_f32(filter_ptr3 + 4);                             \
                                                                 \
  /* outch 0 */                                                  \
  vo0 = vmlaq_lane_f32(vo0, vi0, vget_low_f32(vf00), 0);         \
  vo0 = vmlaq_lane_f32(vo0, vi1, vget_low_f32(vf00), 1);         \
  vo0 = vmlaq_lane_f32(vo0, vi2, vget_high_f32(vf00), 0);        \
  vo0 = vmlaq_lane_f32(vo0, vi3, vget_high_f32(vf00), 1);        \
  vo0 = vmlaq_lane_f32(vo0, vi4, vget_low_f32(vf01), 0);         \
  vo0 = vmlaq_lane_f32(vo0, vi5, vget_low_f32(vf01), 1);         \
  vo0 = vmlaq_lane_f32(vo0, vi6, vget_high_f32(vf01), 0);        \
                                                                 \
  /* outch 1 */                                                  \
  vo1 = vmlaq_lane_f32(vo1, vi0, vget_low_f32(vf10), 0);         \
  vo1 = vmlaq_lane_f32(vo1, vi1, vget_low_f32(vf10), 1);         \
  vo1 = vmlaq_lane_f32(vo1, vi2, vget_high_f32(vf10), 0);        \
  vo1 = vmlaq_lane_f32(vo1, vi3, vget_high_f32(vf10), 1);        \
  vo1 = vmlaq_lane_f32(vo1, vi4, vget_low_f32(vf11), 0);         \
  vo1 = vmlaq_lane_f32(vo1, vi5, vget_low_f32(vf11), 1);         \
  vo1 = vmlaq_lane_f32(vo1, vi6, vget_high_f32(vf11), 0);        \
                                                                 \
  /* outch 2 */                                                  \
  vo2 = vmlaq_lane_f32(vo2, vi0, vget_low_f32(vf20), 0);         \
  vo2 = vmlaq_lane_f32(vo2, vi1, vget_low_f32(vf20), 1);         \
  vo2 = vmlaq_lane_f32(vo2, vi2, vget_high_f32(vf20), 0);        \
  vo2 = vmlaq_lane_f32(vo2, vi3, vget_high_f32(vf20), 1);        \
  vo2 = vmlaq_lane_f32(vo2, vi4, vget_low_f32(vf21), 0);         \
  vo2 = vmlaq_lane_f32(vo2, vi5, vget_low_f32(vf21), 1);         \
  vo2 = vmlaq_lane_f32(vo2, vi6, vget_high_f32(vf21), 0);        \
                                                                 \
  /* outch 3 */                                                  \
  vo3 = vmlaq_lane_f32(vo3, vi0, vget_low_f32(vf30), 0);         \
  vo3 = vmlaq_lane_f32(vo3, vi1, vget_low_f32(vf30), 1);         \
  vo3 = vmlaq_lane_f32(vo3, vi2, vget_high_f32(vf30), 0);        \
  vo3 = vmlaq_lane_f32(vo3, vi3, vget_high_f32(vf30), 1);        \
  vo3 = vmlaq_lane_f32(vo3, vi4, vget_low_f32(vf31), 0);         \
  vo3 = vmlaq_lane_f32(vo3, vi5, vget_low_f32(vf31), 1);         \
  vo3 = vmlaq_lane_f32(vo3, vi6, vget_high_f32(vf31), 0);

#define Conv2dNeonK7x7SnLoadCalc1                                \
  /* load filter (1 outch x 1 height x 4 width) */               \
  float32x4_t vf00, vf01;                                        \
  vf00 = vld1q_f32(filter_ptr0);                                 \
  vf01 = vld1q_f32(filter_ptr0 + 4);                             \
                                                                 \
  /* outch 0 */                                                  \
  vo0 = vmlaq_lane_f32(vo0, vi0, vget_low_f32(vf00), 0);         \
  vo0 = vmlaq_lane_f32(vo0, vi1, vget_low_f32(vf00), 1);         \
  vo0 = vmlaq_lane_f32(vo0, vi2, vget_high_f32(vf00), 0);        \
  vo0 = vmlaq_lane_f32(vo0, vi3, vget_high_f32(vf00), 1);        \
  vo0 = vmlaq_lane_f32(vo0, vi4, vget_low_f32(vf01), 0);         \
  vo0 = vmlaq_lane_f32(vo0, vi5, vget_low_f32(vf01), 1);         \
  vo0 = vmlaq_lane_f32(vo0, vi6, vget_high_f32(vf01), 0);

inline void Conv2dCPUK7x7Calc(const float *in_ptr_base,
                              const float *filter_ptr0,
                              const index_t in_width,
                              const index_t in_channels,
                              const index_t out_height,
                              const index_t out_width,
                              const index_t out_image_size,
                              float *out_ptr0_base,
                              const index_t io,
                              const int stride) {
  for (index_t ih = 0; ih < out_height; ++ih) {
    for (index_t iw = 0; iw < out_width; ++iw) {
      for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
          out_ptr0_base[io * out_image_size + ih * out_width + iw] +=
              in_ptr_base[(ih * stride + i) * in_width + (iw * stride + j)] *
                  filter_ptr0[io * in_channels * 49 + i * 7 + j];
        }
      }
    }
  }
}


// Ho = 1, Wo = 4, Co = 4
void Conv2dNeonK7x7S1(const float *input,
                      const float *filter,
                      const index_t batch,
                      const index_t in_height,
                      const index_t in_width,
                      const index_t in_channels,
                      const index_t out_height,
                      const index_t out_width,
                      const index_t out_channels,
                      float *output) {
  const index_t in_image_size = in_height * in_width;
  const index_t out_image_size = out_height * out_width;
  const index_t in_batch_size = in_channels * in_image_size;
  const index_t out_batch_size = out_channels * out_image_size;

#pragma omp parallel for collapse(2)
  for (index_t b = 0; b < batch; ++b) {
    for (index_t m = 0; m < out_channels; m += 4) {
      if (m + 3 < out_channels) {
        float *out_ptr0_base = output + b * out_batch_size + m * out_image_size;
        float *out_ptr1_base =
          output + b * out_batch_size + (m + 1) * out_image_size;
        float *out_ptr2_base =
            output + b * out_batch_size + (m + 2) * out_image_size;
        float *out_ptr3_base =
            output + b * out_batch_size + (m + 3) * out_image_size;
        for (index_t c = 0; c < in_channels; ++c) {
          const float *in_ptr_base =
              input + b * in_batch_size + c * in_image_size;
          const float *filter_ptr0 = filter + m * in_channels * 49 + c * 49;
          const float *filter_ptr1 =
              filter + (m + 1) * in_channels * 49 + c * 49;
          const float *filter_ptr2 =
              filter + (m + 2) * in_channels * 49 + c * 49;
          const float *filter_ptr3 =
              filter + (m + 3) * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
          for (index_t h = 0; h < out_height; ++h) {
            for (index_t w = 0; w + 3 < out_width; w += 4) {
               // input offset
              index_t in_offset = h * in_width + w;
              // output (4 outch x 1 height x 4 width): vo_outch_height
              float32x4_t vo0, vo1, vo2, vo3;
              // load output
              index_t out_offset = h * out_width + w;
              vo0 = vld1q_f32(out_ptr0_base + out_offset);
              vo1 = vld1q_f32(out_ptr1_base + out_offset);
              vo2 = vld1q_f32(out_ptr2_base + out_offset);
              vo3 = vld1q_f32(out_ptr3_base + out_offset);
              for (index_t r = 0; r < 7; ++r) {
                // input (3 slide)
                float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                float32x4_t vi8;  // for tmp use
                // load input
                vi0 = vld1q_f32(in_ptr_base + in_offset);
                vi4 = vld1q_f32(in_ptr_base + in_offset + 4);
                vi8 = vld1q_f32(in_ptr_base + in_offset + 8);
                vi1 = vextq_f32(vi0, vi4, 1);
                vi2 = vextq_f32(vi0, vi4, 2);
                vi3 = vextq_f32(vi0, vi4, 3);
                vi5 = vextq_f32(vi4, vi8, 1);
                vi6 = vextq_f32(vi4, vi8, 2);

                Conv2dNeonK7x7SnLoadCalc4;

                in_offset += in_width;
                filter_ptr0 += 7;
                filter_ptr1 += 7;
                filter_ptr2 += 7;
                filter_ptr3 += 7;
              }  // r

              vst1q_f32(out_ptr0_base + out_offset, vo0);
              vst1q_f32(out_ptr1_base + out_offset, vo1);
              vst1q_f32(out_ptr2_base + out_offset, vo2);
              vst1q_f32(out_ptr3_base + out_offset, vo3);

              filter_ptr0 -= 49;
              filter_ptr1 -= 49;
              filter_ptr2 -= 49;
              filter_ptr3 -= 49;
            }  // w
          }  // h
#else
          for (index_t io = 0; io < 4; ++io) {
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, io, 1);
          }  // for
#endif
        }  // c
      } else {
        for (index_t mm = m; mm < out_channels; ++mm) {
          float *out_ptr0_base =
              output + b * out_batch_size + mm * out_image_size;
          for (index_t c = 0; c < in_channels; ++c) {
            const float *in_ptr_base =
                input + b * in_batch_size + c * in_image_size;
            const float *filter_ptr0 = filter + mm * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
            for (index_t h = 0; h < out_height; ++h) {
              for (index_t w = 0; w + 3 < out_width; w += 4) {
                // input offset
                index_t in_offset = h * in_width + w;
                // output (1 outch x 1 height x 4 width): vo_outch_height
                float32x4_t vo0;
                // load output
                index_t out_offset = h * out_width + w;
                vo0 = vld1q_f32(out_ptr0_base + out_offset);
                for (index_t r = 0; r < 7; ++r) {
                  // input (3 slide)
                  float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                  float32x4_t vi8;  // for tmp use
                  // load input
                  vi0 = vld1q_f32(in_ptr_base + in_offset);
                  vi4 = vld1q_f32(in_ptr_base + in_offset + 4);
                  vi8 = vld1q_f32(in_ptr_base + in_offset + 8);
                  vi1 = vextq_f32(vi0, vi4, 1);
                  vi2 = vextq_f32(vi0, vi4, 2);
                  vi3 = vextq_f32(vi0, vi4, 3);
                  vi5 = vextq_f32(vi4, vi8, 1);
                  vi6 = vextq_f32(vi4, vi8, 2);

                  Conv2dNeonK7x7SnLoadCalc1;

                  in_offset += in_width;
                  filter_ptr0 += 7;
                }  // r

                vst1q_f32(out_ptr0_base + out_offset, vo0);
                filter_ptr0 -= 49;
              }  // w
            }  // h
#else
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, 0, 1);
#endif
          }  // c
        }  // mm
      }  // if
    }  // m
  }  // b
}

// Ho = 1, Wo = 4, Co = 4
void Conv2dNeonK7x7S2(const float *input,
                      const float *filter,
                      const index_t batch,
                      const index_t in_height,
                      const index_t in_width,
                      const index_t in_channels,
                      const index_t out_height,
                      const index_t out_width,
                      const index_t out_channels,
                      float *output) {
  const index_t in_image_size = in_height * in_width;
  const index_t out_image_size = out_height * out_width;
  const index_t in_batch_size = in_channels * in_image_size;
  const index_t out_batch_size = out_channels * out_image_size;

#pragma omp parallel for collapse(2)
  for (index_t b = 0; b < batch; ++b) {
    for (index_t m = 0; m < out_channels; m += 4) {
      if (m + 3 < out_channels) {
        float *out_ptr0_base = output + b * out_batch_size + m * out_image_size;
        float *out_ptr1_base =
            output + b * out_batch_size + (m + 1) * out_image_size;
        float *out_ptr2_base =
            output + b * out_batch_size + (m + 2) * out_image_size;
        float *out_ptr3_base =
            output + b * out_batch_size + (m + 3) * out_image_size;
        for (index_t c = 0; c < in_channels; ++c) {
          const float *in_ptr_base =
              input + b * in_batch_size + c * in_image_size;
          const float *filter_ptr0 = filter + m * in_channels * 49 + c * 49;
          const float *filter_ptr1 =
              filter + (m + 1) * in_channels * 49 + c * 49;
          const float *filter_ptr2 =
              filter + (m + 2) * in_channels * 49 + c * 49;
          const float *filter_ptr3 =
              filter + (m + 3) * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
          for (index_t h = 0; h < out_height; ++h) {
            for (index_t w = 0; w + 3 < out_width; w += 4) {
              // input offset
              index_t in_h = h * 2;
              index_t in_w = w * 2;
              index_t in_offset = in_h * in_width + in_w;
              // output (4 outch x 1 height x 4 width): vo_outch_height
              float32x4_t vo0, vo1, vo2, vo3;
              // load output
              index_t out_offset = h * out_width + w;
              vo0 = vld1q_f32(out_ptr0_base + out_offset);
              vo1 = vld1q_f32(out_ptr1_base + out_offset);
              vo2 = vld1q_f32(out_ptr2_base + out_offset);
              vo3 = vld1q_f32(out_ptr3_base + out_offset);
              for (index_t r = 0; r < 7; ++r) {
                // input (3 slide)
                float32x4x2_t vvi0, vvi1;  // to de-interleave
                float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                // load input
                // [0.2.4.6, 1.3.5.7]
                vvi0 = vld2q_f32(in_ptr_base + in_offset);
                // [8.10.12.14, 9.11.13.15]
                vvi1 = vld2q_f32(in_ptr_base + in_offset + 8);
                vi0 = vvi0.val[0];  // [0.2.4.6]
                vi1 = vvi0.val[1];  // [1.3.5.7]
                vi2 = vextq_f32(vi0, vvi1.val[0], 1);  // [2.4.6.8]
                vi3 = vextq_f32(vi1, vvi1.val[1], 1);  // [3.5.7.9]
                vi4 = vextq_f32(vi0, vvi1.val[0], 2);  // [4.6.8.10]
                vi5 = vextq_f32(vi1, vvi1.val[1], 2);  // [5.7.9.11]
                vi6 = vextq_f32(vi0, vvi1.val[0], 3);  // [6.8.10.12]

                Conv2dNeonK7x7SnLoadCalc4;

                in_offset += in_width;
                filter_ptr0 += 7;
                filter_ptr1 += 7;
                filter_ptr2 += 7;
                filter_ptr3 += 7;
              }  // r

              vst1q_f32(out_ptr0_base + out_offset, vo0);
              vst1q_f32(out_ptr1_base + out_offset, vo1);
              vst1q_f32(out_ptr2_base + out_offset, vo2);
              vst1q_f32(out_ptr3_base + out_offset, vo3);

              filter_ptr0 -= 49;
              filter_ptr1 -= 49;
              filter_ptr2 -= 49;
              filter_ptr3 -= 49;
            }  // w
          }  // h
#else
          for (index_t io = 0; io < 4; ++io) {
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, io, 2);
          }  // for
#endif
        }  // c
      } else {
        for (index_t mm = m; mm < out_channels; ++mm) {
          float *out_ptr0_base =
              output + b * out_batch_size + mm * out_image_size;
          for (index_t c = 0; c < in_channels; ++c) {
            const float *in_ptr_base =
                input + b * in_batch_size + c * in_image_size;
            const float *filter_ptr0 = filter + mm * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
            for (index_t h = 0; h < out_height; ++h) {
              for (index_t w = 0; w + 3 < out_width; w += 4) {
                // input offset
                index_t in_h = h * 2;
                index_t in_w = w * 2;
                index_t in_offset = in_h * in_width + in_w;
                // output (1 outch x 1 height x 4 width): vo_outch_height
                float32x4_t vo0;
                // load ouput
                index_t out_offset = h * out_width + w;
                vo0 = vld1q_f32(out_ptr0_base + out_offset);
                for (index_t r = 0; r < 7; ++r) {
                  // input (3 slide)
                  float32x4x2_t vvi0, vvi1;  // to de-interleave
                  float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                  // load input
                  // [0.2.4.6, 1.3.5.7]
                  vvi0 = vld2q_f32(in_ptr_base + in_offset);
                  // [8.10.12.14, 9.11.13.15]
                  vvi1 = vld2q_f32(in_ptr_base + in_offset + 8);
                  vi0 = vvi0.val[0];  // [0.2.4.6]
                  vi1 = vvi0.val[1];  // [1.3.5.7]
                  vi2 = vextq_f32(vi0, vvi1.val[0], 1);  // [2.4.6.8]
                  vi3 = vextq_f32(vi1, vvi1.val[1], 1);  // [3.5.7.9]
                  vi4 = vextq_f32(vi0, vvi1.val[0], 2);  // [4.6.8.10]
                  vi5 = vextq_f32(vi1, vvi1.val[1], 2);  // [5.7.9.11]
                  vi6 = vextq_f32(vi0, vvi1.val[0], 3);  // [6.8.10.12]

                  Conv2dNeonK7x7SnLoadCalc1;

                  in_offset += in_width;
                  filter_ptr0 += 7;
                }  // r

                vst1q_f32(out_ptr0_base + out_offset, vo0);
                filter_ptr0 -= 49;
              }  // w
            }  // h
#else
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, 0, 2);
#endif
          }  // c
        }  // mm
      }  // if
    }  // m
  }  // b
}

// Ho = 1, Wo = 4, Co = 4
void Conv2dNeonK7x7S3(const float *input,
                      const float *filter,
                      const index_t batch,
                      const index_t in_height,
                      const index_t in_width,
                      const index_t in_channels,
                      const index_t out_height,
                      const index_t out_width,
                      const index_t out_channels,
                      float *output) {
  const index_t in_image_size = in_height * in_width;
  const index_t out_image_size = out_height * out_width;
  const index_t in_batch_size = in_channels * in_image_size;
  const index_t out_batch_size = out_channels * out_image_size;

#pragma omp parallel for collapse(2)
  for (index_t b = 0; b < batch; ++b) {
    for (index_t m = 0; m < out_channels; m += 4) {
      if (m + 3 < out_channels) {
        float *out_ptr0_base = output + b * out_batch_size + m * out_image_size;
        float *out_ptr1_base =
            output + b * out_batch_size + (m + 1) * out_image_size;
        float *out_ptr2_base =
            output + b * out_batch_size + (m + 2) * out_image_size;
        float *out_ptr3_base =
            output + b * out_batch_size + (m + 3) * out_image_size;
        for (index_t c = 0; c < in_channels; ++c) {
          const float *in_ptr_base =
              input + b * in_batch_size + c * in_image_size;
          const float *filter_ptr0 = filter + m * in_channels * 49 + c * 49;
          const float *filter_ptr1 =
              filter + (m + 1) * in_channels * 49 + c * 49;
          const float *filter_ptr2 =
              filter + (m + 2) * in_channels * 49 + c * 49;
          const float *filter_ptr3 =
              filter + (m + 3) * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
          for (index_t h = 0; h < out_height; ++h) {
            for (index_t w = 0; w + 3 < out_width; w += 4) {
              // input offset
              index_t in_h = h * 3;
              index_t in_w = w * 3;
              index_t in_offset = in_h * in_width + in_w;
              // output (4 outch x 1 height x 4 width): vo_outch_height
              float32x4_t vo0, vo1, vo2, vo3;
              // load output
              index_t out_offset = h * out_width + w;
              vo0 = vld1q_f32(out_ptr0_base + out_offset);
              vo1 = vld1q_f32(out_ptr1_base + out_offset);
              vo2 = vld1q_f32(out_ptr2_base + out_offset);
              vo3 = vld1q_f32(out_ptr3_base + out_offset);
              for (index_t r = 0; r < 7; ++r) {
                // input (3 slide)
                float32x4x3_t vvi0, vvi1;  // to de-interleave
                float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                // load input
                // [0.3.6.9, 1.4.7.10, 2.5.8.11]
                vvi0 = vld3q_f32(in_ptr_base + in_offset);
                // [12.15.xx.xx, 13.xx.xx.xx, 14.xx.xx.xx]
                vvi1 = vld3q_f32(in_ptr_base + in_offset + 12);
                vi0 = vvi0.val[0];  // [0.3.6.9]
                vi1 = vvi0.val[1];  // [1.4.7.10]
                vi2 = vvi0.val[2];  // [2.5.8.11]
                vi3 = vextq_f32(vi0, vvi1.val[0], 1);  // [3.6.9.12]
                vi4 = vextq_f32(vi1, vvi1.val[1], 1);  // [4.7.10.13]
                vi5 = vextq_f32(vi2, vvi1.val[2], 1);  // [5.8.11.14]
                vi6 = vextq_f32(vi0, vvi1.val[0], 2);  // [6.9.12.15]

                Conv2dNeonK7x7SnLoadCalc4;

                in_offset += in_width;
                filter_ptr0 += 7;
                filter_ptr1 += 7;
                filter_ptr2 += 7;
                filter_ptr3 += 7;
              }  // r

              vst1q_f32(out_ptr0_base + out_offset, vo0);
              vst1q_f32(out_ptr1_base + out_offset, vo1);
              vst1q_f32(out_ptr2_base + out_offset, vo2);
              vst1q_f32(out_ptr3_base + out_offset, vo3);

              filter_ptr0 -= 49;
              filter_ptr1 -= 49;
              filter_ptr2 -= 49;
              filter_ptr3 -= 49;
            }  // w
          }  // h
#else
          for (index_t io = 0; io < 4; ++io) {
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, io, 3);
          }  // for
#endif
        }  // c
      } else {
        for (index_t mm = m; mm < out_channels; ++mm) {
          float *out_ptr0_base =
              output + b * out_batch_size + mm * out_image_size;
          for (index_t c = 0; c < in_channels; ++c) {
            const float *in_ptr_base =
                input + b * in_batch_size + c * in_image_size;
            const float *filter_ptr0 = filter + mm * in_channels * 49 + c * 49;
#if defined(MACE_ENABLE_NEON) && !defined(__aarch64__)
            for (index_t h = 0; h < out_height; ++h) {
              for (index_t w = 0; w + 3 < out_width; w += 4) {
                // input offset
                index_t in_h = h * 3;
                index_t in_w = w * 3;
                index_t in_offset = in_h * in_width + in_w;
                // output (1 outch x 1 height x 4 width): vo_outch_height
                float32x4_t vo0;
                // load output
                index_t out_offset = h * out_width + w;
                vo0 = vld1q_f32(out_ptr0_base + out_offset);
                for (index_t r = 0; r < 7; ++r) {
                  // input (3 slide)
                  float32x4x3_t vvi0, vvi1;  // to de-interleave
                  float32x4_t vi0, vi1, vi2, vi3, vi4, vi5, vi6;
                  // load input
                  // [0.3.6.9, 1.4.7.10, 2.5.8.11]
                  vvi0 = vld3q_f32(in_ptr_base + in_offset);
                  // [12.15.xx.xx, 13.xx.xx.xx, 14.xx.xx.xx]
                  vvi1 = vld3q_f32(in_ptr_base + in_offset + 12);
                  vi0 = vvi0.val[0];  // [0.3.6.9]
                  vi1 = vvi0.val[1];  // [1.4.7.10]
                  vi2 = vvi0.val[2];  // [2.5.8.11]
                  vi3 = vextq_f32(vi0, vvi1.val[0], 1);  // [3.6.9.12]
                  vi4 = vextq_f32(vi1, vvi1.val[1], 1);  // [4.7.10.13]
                  vi5 = vextq_f32(vi2, vvi1.val[2], 1);  // [5.8.11.14]
                  vi6 = vextq_f32(vi0, vvi1.val[0], 2);  // [6.9.12.15]

                  Conv2dNeonK7x7SnLoadCalc1;

                  in_offset += in_width;
                  filter_ptr0 += 7;
                }  // r

                vst1q_f32(out_ptr0_base + out_offset, vo0);
                filter_ptr0 -= 49;
              }  // w
            }  // h
#else
            Conv2dCPUK7x7Calc(in_ptr_base, filter_ptr0, in_width, in_channels,
                              out_height, out_width, out_image_size,
                              out_ptr0_base, 0, 3);
#endif
          }  // c
        }  // mm
      }  // if
    }  // m
  }  // b
}

}  // namespace kernels
}  // namespace mace