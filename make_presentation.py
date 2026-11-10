import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.enum.shapes import MSO_SHAPE

def create_presentation():
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)

    # Color Palette
    COLOR_BG = RGBColor(15, 23, 42)        # Slate 900
    COLOR_CARD = RGBColor(30, 41, 59)      # Slate 800
    COLOR_BORDER = RGBColor(51, 65, 85)    # Slate 700
    COLOR_PRIMARY = RGBColor(99, 102, 241) # Indigo accent
    COLOR_CYAN = RGBColor(6, 182, 212)     # Cyan
    COLOR_GREEN = RGBColor(16, 185, 129)   # Emerald
    COLOR_TEXT = RGBColor(248, 250, 252)   # Slate 50
    COLOR_MUTED = RGBColor(148, 163, 184)  # Slate 400

    def apply_slide_bg(slide):
        bg = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, prs.slide_width, prs.slide_height)
        bg.fill.solid()
        bg.fill.fore_color.rgb = COLOR_BG
        bg.line.fill.background()
        return bg

    def add_header(slide, title_text, category_text="DATA STRUCTURES & OS PROJECT"):
        # Category / Presenter Pill
        cat_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.4))
        tf_cat = cat_box.text_frame
        tf_cat.word_wrap = True
        p_cat = tf_cat.paragraphs[0]
        p_cat.text = category_text.upper()
        p_cat.font.size = Pt(11)
        p_cat.font.bold = True
        p_cat.font.color.rgb = COLOR_CYAN

        # Title
        title_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.85), Inches(11.7), Inches(0.8))
        tf_title = title_box.text_frame
        tf_title.word_wrap = True
        p_title = tf_title.paragraphs[0]
        p_title.text = title_text
        p_title.font.size = Pt(28)
        p_title.font.bold = True
        p_title.font.color.rgb = COLOR_TEXT

    blank_layout = prs.slide_layouts[6]

    # ==========================================
    # SLIDE 1: Title Slide (Speaker 1)
    # ==========================================
    s1 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s1)

    # Accent decorative bar
    bar = s1.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), Inches(1.8), Inches(1.2), Inches(0.08))
    bar.fill.solid()
    bar.fill.fore_color.rgb = COLOR_PRIMARY
    bar.line.fill.background()

    t_box = s1.shapes.add_textbox(Inches(0.8), Inches(2.2), Inches(11.7), Inches(2.5))
    tf = t_box.text_frame
    tf.word_wrap = True

    p0 = tf.paragraphs[0]
    p0.text = "COLLEGE DATA STRUCTURES & OPERATING SYSTEMS PROJECT"
    p0.font.size = Pt(13)
    p0.font.bold = True
    p0.font.color.rgb = COLOR_CYAN

    p1 = tf.add_paragraph()
    p1.text = "CPU Process Scheduling Simulator"
    p1.font.size = Pt(40)
    p1.font.bold = True
    p1.font.color.rgb = COLOR_TEXT

    p2 = tf.add_paragraph()
    p2.text = "An interactive scheduling visualizer and comparative benchmark powered by pure C Data Structures."
    p2.font.size = Pt(18)
    p2.font.color.rgb = COLOR_MUTED

    # Presenters Card
    c_card = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(5.0), Inches(11.7), Inches(1.6))
    c_card.fill.solid()
    c_card.fill.fore_color.rgb = COLOR_CARD
    c_card.line.color.rgb = COLOR_BORDER
    c_tf = c_card.text_frame
    c_tf.word_wrap = True
    
    cp1 = c_tf.paragraphs[0]
    cp1.text = "Presentation Structure & Team Breakdown:"
    cp1.font.size = Pt(13)
    cp1.font.bold = True
    cp1.font.color.rgb = COLOR_PRIMARY

    cp2 = c_tf.add_paragraph()
    cp2.text = "• Speaker 1 (Lead): Project Working Background, C Data Structures Architecture & Live Execution Guide\n• Speaker 2: Detailed Algorithm Mechanics (FCFS, Non-Preemptive SJF, Round Robin with Queue)\n• Speaker 3: Gantt Chart Visualization, Metrics Formulas, Performance Comparison & Viva Defense"
    cp2.font.size = Pt(12)
    cp2.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 2: Objectives & Significance (Speaker 1)
    # ==========================================
    s2 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s2)
    add_header(s2, "Project Objectives & Real-World Relevance", "SPEAKER 1: PROJECT OVERVIEW")

    cards_data_s2 = [
        ("The Core Problem", "CPU scheduling is central to multi-programming. Visualizing how processes queue, preempt, and incur idle gaps is difficult from textbook equations alone.", COLOR_PRIMARY),
        ("Pure C Data Structures", "Implement FCFS, SJF, and Round Robin using authentic C structs, arrays, and queues with zero third-party dependencies.", COLOR_CYAN),
        ("Dual Execution Modes", "Provide both a standalone C terminal CLI for viva code inspection and an interactive web simulator with Gantt charts.", COLOR_GREEN),
        ("Accurate Metrics", "Calculate exact Completion, Turnaround, and Waiting Times, factoring in CPU idle periods when no jobs are ready.", COLOR_PRIMARY)
    ]

    for i, (title, desc, color) in enumerate(cards_data_s2):
        x = Inches(0.8 + (i % 2) * 5.95)
        y = Inches(1.9 + (i // 2) * 2.5)
        card = s2.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x, y, Inches(5.65), Inches(2.2))
        card.fill.solid()
        card.fill.fore_color.rgb = COLOR_CARD
        card.line.color.rgb = COLOR_BORDER
        ctf = card.text_frame
        ctf.word_wrap = True
        
        pt = ctf.paragraphs[0]
        pt.text = title
        pt.font.size = Pt(16)
        pt.font.bold = True
        pt.font.color.rgb = color

        pd = ctf.add_paragraph()
        pd.text = desc
        pd.font.size = Pt(13)
        pd.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 3: Architecture & Data Structures in C (Speaker 1)
    # ==========================================
    s3 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s3)
    add_header(s3, "Under the Hood: C Data Structures Design", "SPEAKER 1: WORKING BACKGROUND")

    ds_cards = [
        ("1. struct Process", 
         "Tracks process lifecycle:\n• char id[10] (e.g. P1, P2)\n• int at, bt (Arrival & Burst)\n• int ct, tat, wt (Computed metrics)\n• int rem_bt (Remaining time for RR)\n• int completed (Flag)"),
        ("2. FIFO Queue DS", 
         "Circular/Array Queue for Round Robin:\n• int queue[500]\n• push(val) & pop() operations\n• isQueueEmpty() check\n• Guarantees fair, round-order execution of active processes"),
        ("3. struct Gantt", 
         "Execution interval tracking:\n• char id[10] (Process ID or 'IDLE')\n• int start, end (Time slice)\n• Automatically detects CPU idle gaps and merges consecutive runs of the same process")
    ]

    for i, (title, desc) in enumerate(ds_cards):
        x = Inches(0.8 + i * 4.0)
        card = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x, Inches(2.0), Inches(3.75), Inches(4.7))
        card.fill.solid()
        card.fill.fore_color.rgb = COLOR_CARD
        card.line.color.rgb = COLOR_PRIMARY if i == 1 else COLOR_BORDER
        ctf = card.text_frame
        ctf.word_wrap = True

        pt = ctf.paragraphs[0]
        pt.text = title
        pt.font.size = Pt(17)
        pt.font.bold = True
        pt.font.color.rgb = COLOR_CYAN if i != 1 else COLOR_GREEN

        pd = ctf.add_paragraph()
        pd.text = desc
        pd.font.size = Pt(13)
        pd.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 4: How to Run the Programs (Speaker 1)
    # ==========================================
    s4 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s4)
    add_header(s4, "Program Execution Guide: Terminal CLI & Web UI", "SPEAKER 1: DEMO GUIDE")

    # Mode 1: Terminal
    m1 = s4.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    m1.fill.solid()
    m1.fill.fore_color.rgb = COLOR_CARD
    m1.line.color.rgb = COLOR_BORDER
    tf1 = m1.text_frame
    tf1.word_wrap = True

    p = tf1.paragraphs[0]
    p.text = "Option A: Pure C Terminal Program"
    p.font.size = Pt(18)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN

    p = tf1.add_paragraph()
    p.text = "File: cli_scheduler.c (Standard C, Zero Dependencies)\n\n" \
             "1. Compile with MinGW GCC:\n" \
             "   gcc -Wall -O2 cli_scheduler.c -o cli_scheduler.exe\n\n" \
             "2. Run in Terminal:\n" \
             "   .\\cli_scheduler.exe\n\n" \
             "Features:\n" \
             "• Interactive numbered menu (1 to 6)\n" \
             "• Add custom processes or use preloaded samples\n" \
             "• Displays ASCII Gantt Chart & metrics table\n" \
             "• Ideal for lab viva and direct code inspection"
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # Mode 2: Web UI
    m2 = s4.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    m2.fill.solid()
    m2.fill.fore_color.rgb = COLOR_CARD
    m2.line.color.rgb = COLOR_BORDER
    tf2 = m2.text_frame
    tf2.word_wrap = True

    p = tf2.paragraphs[0]
    p.text = "Option B: Full Interactive Web Simulator"
    p.font.size = Pt(18)
    p.font.bold = True
    p.font.color.rgb = COLOR_GREEN

    p = tf2.add_paragraph()
    p.text = "HTML, CSS, JS + C Backend via Python Bridge\n\n" \
             "1. Start the Server:\n" \
             "   python server.py\n" \
             "   (Auto-compiles C engine if needed)\n\n" \
             "2. Open in Browser:\n" \
             "   http://localhost:5000\n\n" \
             "Features:\n" \
             "• Modern dark theme & responsive controls\n" \
             "• Dynamic process row addition/deletion\n" \
             "• Multi-color animated Gantt chart with timeline ruler\n" \
             "• Compare All mode with comparative bar charts"
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 5: FCFS Algorithm (Speaker 2)
    # ==========================================
    s5 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s5)
    add_header(s5, "Algorithm 1: First Come First Serve (FCFS)", "SPEAKER 2: ALGORITHM LOGIC")

    c_fcfs_1 = s5.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_fcfs_1.fill.solid()
    c_fcfs_1.fill.fore_color.rgb = COLOR_CARD
    c_fcfs_1.line.color.rgb = COLOR_BORDER
    tf = c_fcfs_1.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Core Principles & Mechanics"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_PRIMARY
    p = tf.add_paragraph()
    p.text = "• Scheduling Type: Non-Preemptive\n" \
             "• Policy: Strict FIFO arrival order (First in, First out)\n\n" \
             "Step-by-Step C Implementation:\n" \
             "1. Sort processes primarily by Arrival Time (at).\n" \
             "2. Track current_time starting at 0.\n" \
             "3. If current_time < p[i].at:\n" \
             "   Record 'IDLE' block; set current_time = p[i].at.\n" \
             "4. Execute process to completion:\n" \
             "   current_time += p[i].bt;\n" \
             "5. Compute CT = current_time, TAT, and WT."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    c_fcfs_2 = s5.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_fcfs_2.fill.solid()
    c_fcfs_2.fill.fore_color.rgb = COLOR_CARD
    c_fcfs_2.line.color.rgb = COLOR_BORDER
    tf = c_fcfs_2.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Analysis & The Convoy Effect"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN
    p = tf.add_paragraph()
    p.text = "Advantages:\n" \
             "• Simple to understand and implement.\n" \
             "• Minimal scheduling overhead.\n\n" \
             "Key Drawback: The Convoy Effect\n" \
             "• If a CPU-heavy process arrives first (e.g., Burst = 20), all subsequent short jobs (e.g., Burst = 2) are forced to wait.\n" \
             "• Result: High Average Waiting Time and poor system throughput.\n\n" \
             "Solution:\n" \
             "• Shortest Job First (SJF) or Round Robin to interleave execution."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 6: SJF Algorithm (Speaker 2)
    # ==========================================
    s6 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s6)
    add_header(s6, "Algorithm 2: Shortest Job First (Non-Preemptive)", "SPEAKER 2: ALGORITHM LOGIC")

    c_sjf_1 = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_sjf_1.fill.solid()
    c_sjf_1.fill.fore_color.rgb = COLOR_CARD
    c_sjf_1.line.color.rgb = COLOR_BORDER
    tf = c_sjf_1.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Selection Logic in C"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_GREEN
    p = tf.add_paragraph()
    p.text = "• Scheduling Type: Non-Preemptive\n" \
             "• Policy: Pick arrived process with minimum Burst Time\n\n" \
             "How the C loop works:\n" \
             "1. Maintain current_time and completed count.\n" \
             "2. Loop through all processes where at <= current_time and completed == 0.\n" \
             "3. Select index with smallest bt.\n" \
             "4. If no process is ready (idx == -1):\n" \
             "   Advance current_time to next arrival (IDLE time).\n" \
             "5. Run selected process to completion, update CT, and mark completed = 1."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    c_sjf_2 = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_sjf_2.fill.solid()
    c_sjf_2.fill.fore_color.rgb = COLOR_CARD
    c_sjf_2.line.color.rgb = COLOR_BORDER
    tf = c_sjf_2.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Evaluation & Trade-offs"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_PRIMARY
    p = tf.add_paragraph()
    p.text = "Provable Optimality:\n" \
             "• SJF gives the minimum average waiting time for any given set of processes.\n\n" \
             "Limitations in Real Systems:\n" \
             "1. Burst Time Prediction:\n" \
             "   In real OS kernels, the CPU cannot know future burst times in advance.\n" \
             "2. Starvation Risk:\n" \
             "   Long processes may wait indefinitely if short processes keep arriving."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 7: Round Robin Algorithm (Speaker 2)
    # ==========================================
    s7 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s7)
    add_header(s7, "Algorithm 3: Round Robin (RR) with Queue Data Structure", "SPEAKER 2: ALGORITHM LOGIC")

    c_rr_1 = s7.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_rr_1.fill.solid()
    c_rr_1.fill.fore_color.rgb = COLOR_CARD
    c_rr_1.line.color.rgb = COLOR_BORDER
    tf = c_rr_1.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Preemptive Queue Execution"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN
    p = tf.add_paragraph()
    p.text = "• Scheduling Type: Preemptive, Time-Sharing\n" \
             "• Time Quantum (q): Maximum continuous CPU slice\n\n" \
             "Data Structure Mechanics:\n" \
             "1. Initialize FIFO Queue: queue[], front=0, rear=-1.\n" \
             "2. Push processes arriving at current_time into Queue.\n" \
             "3. Pop process: curr = pop().\n" \
             "4. Execute for slice = min(quantum, rem_bt).\n" \
             "5. Enqueue any newly arrived processes during this slice.\n" \
             "6. If rem_bt > 0: push(curr) back to rear;\n" \
             "   Else: mark completed and record CT."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    c_rr_2 = s7.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_rr_2.fill.solid()
    c_rr_2.fill.fore_color.rgb = COLOR_CARD
    c_rr_2.line.color.rgb = COLOR_BORDER
    tf = c_rr_2.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Impact of Time Quantum (q)"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_GREEN
    p = tf.add_paragraph()
    p.text = "Why Time Quantum is Critical:\n\n" \
             "• If Quantum is very large (q >= max(BT)):\n" \
             "  Round Robin degenerates into standard FCFS.\n\n" \
             "• If Quantum is very small (e.g. q = 1 unit):\n" \
             "  Frequent context switching introduces heavy overhead in practical systems.\n\n" \
             "• Optimal Setting:\n" \
             "  80% of CPU bursts should be shorter than the time quantum to balance fairness and throughput."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 8: Gantt Chart & Formulas (Speaker 3)
    # ==========================================
    s8 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s8)
    add_header(s8, "Visualization & Performance Evaluation Metrics", "SPEAKER 3: METRICS & FORMULAS")

    c_m1 = s8.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_m1.fill.solid()
    c_m1.fill.fore_color.rgb = COLOR_CARD
    c_m1.line.color.rgb = COLOR_BORDER
    tf = c_m1.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Mathematical Formulas Implemented"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_PRIMARY
    p = tf.add_paragraph()
    p.text = "1. Turnaround Time (TAT):\n" \
             "   TAT = Completion Time (CT) - Arrival Time (AT)\n" \
             "   Total time a process spends from arrival to finish.\n\n" \
             "2. Waiting Time (WT):\n" \
             "   WT = Turnaround Time (TAT) - Burst Time (BT)\n" \
             "   Time spent sitting in the ready queue.\n\n" \
             "3. Averages:\n" \
             "   Avg TAT = (Sum of all TAT) / n\n" \
             "   Avg WT = (Sum of all WT) / n\n\n" \
             "4. CPU Utilization:\n" \
             "   (Total Busy Time / Total Schedule Time) * 100%"
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    c_m2 = s8.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_m2.fill.solid()
    c_m2.fill.fore_color.rgb = COLOR_CARD
    c_m2.line.color.rgb = COLOR_BORDER
    tf = c_m2.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Gantt Chart Representation"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN
    p = tf.add_paragraph()
    p.text = "What the Gantt Chart Shows:\n" \
             "• Horizontal timeline mapping each CPU burst.\n" \
             "• Start and Finish timestamps on each boundary.\n" \
             "• Handles and clearly labels CPU IDLE gaps.\n\n" \
             "Dual Output Views:\n" \
             "• Terminal (CLI): Formatted ASCII boxes with time labels.\n" \
             "• Web UI: Proportional color blocks, hovering tooltips, and dynamic duration markers."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 9: Results Comparison (Speaker 3)
    # ==========================================
    s9 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s9)
    add_header(s9, "Comparative Analysis on Standard Workload", "SPEAKER 3: PERFORMANCE COMPARISON")

    # Table Card
    t_card = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(11.7), Inches(2.4))
    t_card.fill.solid()
    t_card.fill.fore_color.rgb = COLOR_CARD
    t_card.line.color.rgb = COLOR_BORDER
    tf = t_card.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Benchmark Test: 4 Processes [P1(AT=0, BT=5), P2(AT=1, BT=3), P3(AT=2, BT=8), P4(AT=3, BT=6)]"
    p.font.size = Pt(14)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN

    p = tf.add_paragraph()
    p.text = "Algorithm                    Avg Waiting Time (WT)    Avg Turnaround Time (TAT)    Verdict\n" \
             "----------------------------------------------------------------------------------------------------\n" \
             "1. FCFS                     5.75 ms                  11.25 ms                     Simple, Convoy Effect\n" \
             "2. SJF (Non-Preemptive)     5.25 ms                  10.75 ms                     * OPTIMAL (Lowest WT)\n" \
             "3. Round Robin (q=2)        9.75 ms                  15.25 ms                     Fair, High Preemption"
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    # Insight Cards
    ins_1 = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(4.6), Inches(5.65), Inches(2.1))
    ins_1.fill.solid()
    ins_1.fill.fore_color.rgb = COLOR_CARD
    ins_1.line.color.rgb = COLOR_GREEN
    tf1 = ins_1.text_frame
    tf1.word_wrap = True
    p = tf1.paragraphs[0]
    p.text = "Key Observation: SJF is Optimal"
    p.font.size = Pt(15)
    p.font.bold = True
    p.font.color.rgb = COLOR_GREEN
    p = tf1.add_paragraph()
    p.text = "SJF scheduled P2 (Burst 3) immediately after P1, reducing queue wait times for subsequent jobs and achieving the lowest Average WT (5.25 ms)."
    p.font.size = Pt(12)
    p.font.color.rgb = COLOR_TEXT

    ins_2 = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(4.6), Inches(5.65), Inches(2.1))
    ins_2.fill.solid()
    ins_2.fill.fore_color.rgb = COLOR_CARD
    ins_2.line.color.rgb = COLOR_PRIMARY
    tf2 = ins_2.text_frame
    tf2.word_wrap = True
    p = tf2.paragraphs[0]
    p.text = "Key Observation: Round Robin Trade-off"
    p.font.size = Pt(15)
    p.font.bold = True
    p.font.color.rgb = COLOR_PRIMARY
    p = tf2.add_paragraph()
    p.text = "Round Robin produced higher turnaround (15.25 ms) due to time-slicing, but provided the fastest initial response time and absolute fairness."
    p.font.size = Pt(12)
    p.font.color.rgb = COLOR_TEXT

    # ==========================================
    # SLIDE 10: Conclusion & Viva Prep (Speaker 3 + Speaker 1)
    # ==========================================
    s10 = prs.slides.add_slide(blank_layout)
    apply_slide_bg(s10)
    add_header(s10, "Summary, Learnings & Viva Defense", "CONCLUSION & DEFENSE")

    c_end1 = s10.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_end1.fill.solid()
    c_end1.fill.fore_color.rgb = COLOR_CARD
    c_end1.line.color.rgb = COLOR_BORDER
    tf = c_end1.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Project Takeaways"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_CYAN
    p = tf.add_paragraph()
    p.text = "• Applied C Data Structures (structures, circular arrays, queues) to solve real OS-level scheduling tasks.\n\n" \
             "• Built both an accessible CLI and a modern Web UI to cater to practical evaluations.\n\n" \
             "• Validated mathematical formulas against textbook standards across various workloads.\n\n" \
             "• Successfully handled edge cases: CPU idle intervals, simultaneous arrivals, and dynamic time slices."
    p.font.size = Pt(13)
    p.font.color.rgb = COLOR_TEXT

    c_end2 = s10.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.8), Inches(2.0), Inches(5.65), Inches(4.7))
    c_end2.fill.solid()
    c_end2.fill.fore_color.rgb = COLOR_CARD
    c_end2.line.color.rgb = COLOR_GREEN
    tf = c_end2.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Top Viva Questions Ready"
    p.font.size = Pt(17)
    p.font.bold = True
    p.font.color.rgb = COLOR_GREEN
    p = tf.add_paragraph()
    p.text = "Q1: Why use a Queue for Round Robin?\n" \
             "Ans: Ensures FIFO ordering: active process executes for one quantum and re-enters the rear in O(1) time.\n\n" \
             "Q2: Why is SJF not used directly in Windows/Linux?\n" \
             "Ans: Future burst times cannot be known in advance for general processes.\n\n" \
             "Q3: What happens if Quantum is too large/small?\n" \
             "Ans: Too large -> degenerates to FCFS. Too small -> excessive context switching overhead.\n\n" \
             "Thank You! We are now open for questions."
    p.font.size = Pt(12)
    p.font.color.rgb = COLOR_TEXT

    output_path = os.path.join(os.path.dirname(__file__), "CPU_Scheduling_Simulator_Presentation.pptx")
    prs.save(output_path)
    print(f"Presentation successfully saved to: {output_path}")

if __name__ == "__main__":
    create_presentation()
