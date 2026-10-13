from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PROGRAM = os.environ.get('PROGRAM', str(ROOT/'allocate'))

class SchedulerTests(unittest.TestCase):
    def schedule(self, text, processors=1, custom=False, valid=True):
        with tempfile.NamedTemporaryFile(mode='w', dir=ROOT/'.build', suffix='.txt') as f:
            f.write(text); f.flush()
            r = subprocess.run([PROGRAM,'-f',f.name,'-p',str(processors)]+(['-c'] if custom else []),
                capture_output=True,text=True,timeout=5)
        self.assertNotIn('AddressSanitizer',r.stderr)
        self.assertNotIn('runtime error:',r.stderr)
        self.assertEqual(r.returncode==0,valid,r.stderr)
        return r.stdout

    def test_one_and_idle_cpu(self):
        self.assertIn('Makespan 3',self.schedule('0 1 3 n\n'))
        out=self.schedule('0 1 1 n\n5 2 1 n\n')
        self.assertIn('5,RUNNING,pid=2',out)
        self.assertIn('6,FINISHED,pid=2',out)
        self.assertIn('Makespan 6',out)

    def test_equal_arrival_preemption_and_simultaneous_finish(self):
        out=self.schedule('0 2 2 n\n0 1 2 n\n')
        self.assertTrue(out.startswith('0,RUNNING,pid=1,'))
        out=self.schedule('0 1 5 n\n1 2 1 n\n')
        self.assertIn('1,RUNNING,pid=2',out)
        self.assertIn('2,RUNNING,pid=1',out)
        out=self.schedule('0 1 2 n\n0 2 2 n\n',2)
        self.assertIn('2,FINISHED,pid=1,proc_remaining=0\n2,FINISHED,pid=2,proc_remaining=0',out)

    def test_parallel_and_custom_paths(self):
        self.assertIn('Makespan 3',self.schedule('0 1 4 p\n',2))
        self.assertIn('Makespan 4',self.schedule('0 1 5 p\n',3,True))
        self.assertIn('proc_remaining=0',self.schedule('0 1 5 p\n',3,True))
        for processors in (1,2,3,16,1024):
            for custom in (False,True):
                out=self.schedule('0 1 1 p\n',processors,custom)
                self.assertIn('proc_remaining=0',out)

    def test_exact_ids_large_arrival_and_split_suffixes(self):
        out=self.schedule('16777217 16777217 1 n\n')
        self.assertIn('16777217,RUNNING,pid=16777217,',out)
        out=self.schedule('0 1 20 p\n',16)
        self.assertIn('pid=1.10,',out)
        self.assertIn('pid=1.15,',out)

    def test_invalid_records_and_arguments(self):
        for text in ['', '0 1\n', '0 1 0 n\n', '-1 1 1 n\n', '0 1 1 x\n',
            '0 1 1 n extra\n', '0 1 1 n\n0 1 2 n\n', '2 1 1 n\n1 2 1 n\n',
            '0 1 999999999999999999999 n\n']:
            self.schedule(text,valid=False)
        for processors in (0,-1,1025):
            self.schedule('0 1 1 n\n',processors,valid=False)
        for args in [[],['-f'],['-p','1','-f'],['-f','missing','-p','abc']]:
            r=subprocess.run([PROGRAM,*args],capture_output=True,timeout=5)
            self.assertNotEqual(r.returncode,0)

    def test_independent_srtf_oracle(self):
        import random
        import re
        rng=random.Random(2026)
        for _ in range(30):
            jobs=sorted([(rng.randrange(8),i+1,rng.randrange(1,8),rng.choice('np')) for i in range(10)])
            processors=rng.choice((1,2,3,8))
            queues=[[] for _ in range(processors)]
            remaining={job[1]:0 for job in jobs}
            finished={}
            now=0
            while len(finished)<len(jobs):
                for arrival,pid,duration,kind in sorted(jobs,key=lambda j:(j[2],j[1])):
                    if arrival==now:
                        k=min(duration,processors) if kind=='p' and processors>1 else 1
                        child_duration=(duration+processors-1)//processors+1 if kind=='p' and processors>1 else duration
                        remaining[pid]=k
                        for sub in range(k):
                            cpu=min(range(processors),key=lambda c:(sum(x[1] for x in queues[c]),c))
                            queues[cpu].append([pid,child_duration,sub])
                for queue in queues:
                    for job in list(queue):
                        if job[1]==0:
                            queue.remove(job);remaining[job[0]]-=1
                            if remaining[job[0]]==0:finished[job[0]]=now
                if len(finished)==len(jobs):break
                for queue in queues:
                    if queue:min(queue,key=lambda j:(j[1],j[0],j[2]))[1]-=1
                now+=1
                self.assertLess(now,1000)
            out=self.schedule(''.join(f'{a} {pid} {d} {k}\n' for a,pid,d,k in jobs),processors)
            actual={int(pid):int(time) for time,pid in re.findall(r'(\d+),FINISHED,pid=(\d+),',out)}
            self.assertEqual(actual,finished)
            self.assertIn(f'Makespan {now}',out)

    def test_examples_and_bounded_repetition(self):
        text='0 4 30 n\n3 2 40 n\n5 1 20 n\n20 3 30 n\n'
        out=self.schedule(text)
        self.assertIn('Turnaround time 62',out)
        self.assertIn('Makespan 120',out)
        for _ in range(5): self.assertEqual(self.schedule(text),out)
        for custom in (False,True):
            for processors in (1,2,4):
                text=''.join(f'{i//5} {i+1} {i%7+1} {"p" if i%3==0 else "n"}\n' for i in range(100))
                out=self.schedule(text,processors,custom)
                self.assertEqual(out.count('FINISHED'),100)
                self.assertIn('proc_remaining=0',out)

if __name__=='__main__':unittest.main()
