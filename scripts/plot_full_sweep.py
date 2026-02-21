#!/usr/bin/env python3

import json
import sys
import os

try:
    import matplotlib.pyplot as plt
    import numpy as np
except ImportError as e:
    print(f"Error: Required library not found: {e}")
    print("Install with: pip3 install matplotlib numpy")
    sys.exit(1)

def main():
    filename = sys.argv[1] if len(sys.argv) > 1 else 'results/full_snr_sweep.json'
    
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found")
        sys.exit(1)
    
    with open(filename, 'r') as f:
        data = json.load(f)
    
    metadata = data['metadata']
    results = data['results']
    
    by_length = {}
    for r in results:
        n = r['num_of_pucch_f2_bits']
        if n not in by_length:
            by_length[n] = {'snr': [], 'bler': []}
        by_length[n]['snr'].append(r['snr_db'])
        by_length[n]['bler'].append(r['bler'])
    
    for n in by_length:
        pairs = sorted(zip(by_length[n]['snr'], by_length[n]['bler']))
        by_length[n]['snr'] = [p[0] for p in pairs]
        by_length[n]['bler'] = [p[1] for p in pairs]
    
    plt.figure(figsize=(10, 6))
    colors = plt.cm.viridis(np.linspace(0, 1, len(by_length)))
    
    for i, (n, vals) in enumerate(sorted(by_length.items())):
        plt.semilogy(vals['snr'], vals['bler'], 'o-', 
                    label=f'n = {n} bits', markersize=6, linewidth=2,
                    color=colors[i])
    
    plt.grid(True, which='both', linestyle='--', alpha=0.7)
    plt.xlabel('SNR (dB)', fontsize=12)
    plt.ylabel('BLER', fontsize=12)
    plt.title('PUCCH Format 2: BLER vs SNR\n' + 
              f"Iterations: {metadata['iterations']}, Step: {metadata['snr_range']['step']} dB",
              fontsize=14)
    plt.legend(fontsize=10, loc='upper right')
    plt.xlim(metadata['snr_range']['start'] - 1, metadata['snr_range']['end'] + 1)
    plt.ylim(1e-3, 1)
    
    output_png = filename.replace('.json', '.png')
    plt.savefig(output_png, dpi=300, bbox_inches='tight')
    print(f"✓ Plot saved to {output_png}")
    plt.show()

if __name__ == '__main__':
    main()