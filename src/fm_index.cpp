#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include <sais.h>

using namespace std;

class SaisSaca {
  public:
    SaisSaca() {}

    int32_t BuildBWT(const vector<string>& strings, size_t depth, uint8_t** bwt, size_t* bwt_size) {
        size_t num_strings = strings.size();
        *bwt_size = num_strings;
        for (string s : strings) {
            *bwt_size = *bwt_size + s.size();
        }

        sa_int32_t* super_string = new sa_int32_t[*bwt_size];
        uint32_t pos = 0;
        for (uint32_t sidx = 0; sidx < num_strings; ++sidx) {
            super_string[pos++] = (sidx ? sidx - 1 : num_strings - 1);
            for (char c : strings[(sidx + 1) % num_strings]) {
                super_string[pos++] = (sa_int32_t)c + num_strings - 1;
            }
        }

        sa_int32_t* bwt32 = new sa_int32_t[*bwt_size];

        if ((int32_t)sais_i32_bwt(super_string, bwt32, NULL, (sa_int32_t)*bwt_size, depth) < 0) {
            return -1;
        }

        *bwt = new uint8_t[*bwt_size];
        for (pos = 0; pos < *bwt_size; ++pos) {
            sa_int32_t c = bwt32[pos];
            (*bwt)[pos] = (c < (int32_t)num_strings ? 0 : c - num_strings + 1);
        }

        return 0;
    }
};

class FMIndex {
  public:
    FMIndex(const uint8_t* bwt, size_t size, size_t depth, size_t bucket_size) :
            bwt_(bwt), size_(size), depth_(depth), bucket_size_(bucket_size),
            num_buckets_(size_ / bucket_size_ + 1) {
        Init();
    }

    uint32_t Rank(uint8_t chr, uint32_t pos) {
        uint32_t count = 0;
        for (uint32_t idx = pos % bucket_size_; idx > 0; --idx) {
            count += (bwt_[pos - idx] == chr ? 1 : 0);
        }
        uint32_t* bucket = count_buckets_ + (pos / bucket_size_ * depth_);
        if (chr > 0) {
            count += bucket[chr - 1];
        } else if (chr == 0) {
            count += (pos / bucket_size_) * bucket_size_;
            for (uint32_t cidx = 0; cidx < depth_; ++cidx) {
                count -= bucket[cidx];
            }
        }
        return count;
    }

    ~FMIndex() {
        delete [] count_buckets_;
    }

  private:
    void Init() {
        count_buckets_ = new uint32_t[num_buckets_ * depth_];
        memset(count_buckets_, 0, num_buckets_ * depth_ * sizeof(uint32_t));

        for (uint32_t bidx = 1; bidx < num_buckets_; ++bidx) {
            uint32_t* curr_bucket = count_buckets_ + bidx * depth_;
            uint32_t* prev_bucket = count_buckets_ + (bidx - 1) * depth_;

            for (uint32_t cidx = 0; cidx < depth_; ++cidx) {
                curr_bucket[cidx] = prev_bucket[cidx];
            }
            for (uint32_t pos = 0; pos < bucket_size_; ++pos) {
                uint8_t chr = bwt_[(bidx - 1) * bucket_size_ + pos];
                if (chr > 0) {
                    ++curr_bucket[chr - 1];
                }
            }
        }
    }

    const uint8_t* bwt_;
    const size_t size_;
    const size_t depth_;
    const size_t bucket_size_;
    const size_t num_buckets_;
    uint32_t* count_buckets_;
};

int main() {
    vector<string> strings;
    SaisSaca sais;

    char tmp[1000];
    while ((scanf("%s", tmp)) != EOF) {
        strings.push_back(tmp);
    }

    uint8_t* bwt = NULL;
    size_t bwt_size = 0;

    if (sais.BuildBWT(strings, 256, &bwt, &bwt_size) != 0) {
        printf("fail\n");
        return 1;
    }

    for (uint32_t pos = 0; pos < bwt_size; ++pos) {
        if (bwt[pos] > 0) {
            printf("%c", bwt[pos]);
        } else {
            printf("0");
        }
    }
    printf("\n");

    FMIndex fmi(bwt, bwt_size, 256, 64);

    for (uint8_t chr = 0; chr <= 'z'; ++chr) {
        uint32_t cnt = 0;
        for (uint32_t pos = 0; pos <= bwt_size; ++pos) {
            if (pos && bwt[pos - 1] == chr) {
                ++cnt;
            }
            uint32_t rank = fmi.Rank(chr, pos);
            if (cnt != fmi.Rank(chr, pos)) {
                printf("%d %d: %d %d\n", chr, pos, cnt, rank);
            }
        }
    }

    return 0;
}
