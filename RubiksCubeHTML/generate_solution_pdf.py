import os
import math
from PIL import Image, ImageDraw, ImageFont
from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image as RLImage, Table, TableStyle, PageBreak
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors

# Directory settings
OUTPUT_DIR = r"c:\@_Programming\MM_RubiksCubeSolver\RubiksCubeHTML"
SCRATCH_DIR = r"C:\Users\prash\.gemini\antigravity\brain\2495d75e-431d-4f68-81cd-86ec91d2769d\scratch"

# Cube coloring palette
PALETTE = {
    'W': (248, 249, 250),   # White
    'Y': (252, 211, 77),    # Yellow
    'B': (37, 99, 235),     # Blue
    'R': (239, 68, 68),     # Red
    'G': (16, 185, 129),    # Green
    'O': (249, 115, 22),    # Orange
    'X': (209, 213, 219)    # Grey (unsolved)
}

def shade_color(color, factor):
    return tuple(max(0, min(255, int(c * factor))) for c in color)

def draw_isometric_cube(up_state, left_state, right_state, arrows=None, filename="cube.png"):
    """
    Renders a 3D isometric Rubik's Cube.
    up_state, left_state, right_state are 3x3 matrices of colors ('W', 'Y', 'B', 'R', 'G', 'O', 'X')
    arrows is a list of tuples: (face_name, direction_clockwise_1_or_minus1, arrow_color_rgb)
    """
    img = Image.new("RGBA", (400, 400), (255, 255, 255, 0))
    draw = ImageDraw.Draw(img)

    cx, cy = 200, 130
    w = 40
    h = 23
    l = 46

    # 1. Calculate grid vertices for Up face (top)
    x_grid = [[0]*4 for _ in range(4)]
    y_grid = [[0]*4 for _ in range(4)]
    for i in range(4):
        for j in range(4):
            x_grid[i][j] = cx + (i - 1.5) * w - (j - 1.5) * w
            y_grid[i][j] = cy + (i - 1.5) * h + (j - 1.5) * h

    # 2. Draw Up face (top)
    for i in range(3):
        for j in range(3):
            p0 = (x_grid[i][j], y_grid[i][j])
            p1 = (x_grid[i+1][j], y_grid[i+1][j])
            p2 = (x_grid[i+1][j+1], y_grid[i+1][j+1])
            p3 = (x_grid[i][j+1], y_grid[i][j+1])
            
            color_key = up_state[i][j]
            fill_color = PALETTE.get(color_key, PALETTE['X'])
            # Top face lighting: 100% brightness
            draw.polygon([p0, p1, p2, p3], fill=fill_color, outline=(0, 0, 0), width=2)

    # 3. Draw Left face (Front-Left in isometric view)
    # The top edge matches the Up face vertices at i = 3
    x_left = [[0]*4 for _ in range(4)]
    y_left = [[0]*4 for _ in range(4)]
    for j in range(4):
        for k in range(4):
            x_left[j][k] = x_grid[3][j]
            y_left[j][k] = y_grid[3][j] + k * l

    for j in range(3):
        for k in range(3):
            p0 = (x_left[j][k], y_left[j][k])
            p1 = (x_left[j+1][k], y_left[j+1][k])
            p2 = (x_left[j+1][k+1], y_left[j+1][k+1])
            p3 = (x_left[j][k+1], y_left[j][k+1])

            color_key = left_state[j][k]
            fill_color = shade_color(PALETTE.get(color_key, PALETTE['X']), 0.85) # 85% brightness
            draw.polygon([p0, p1, p2, p3], fill=fill_color, outline=(0, 0, 0), width=2)

    # 4. Draw Right face (Front-Right in isometric view)
    # The top edge matches the Up face vertices at j = 3
    x_right = [[0]*4 for _ in range(4)]
    y_right = [[0]*4 for _ in range(4)]
    for i in range(4):
        for k in range(4):
            x_right[i][k] = x_grid[i][3]
            y_right[i][k] = y_grid[i][3] + k * l

    for i in range(3):
        for k in range(3):
            p0 = (x_right[i][k], y_right[i][k])
            p1 = (x_right[i+1][k], y_right[i+1][k])
            p2 = (x_right[i+1][k+1], y_right[i+1][k+1])
            p3 = (x_right[i][k+1], y_right[i][k+1])

            color_key = right_state[i][k]
            fill_color = shade_color(PALETTE.get(color_key, PALETTE['X']), 0.70) # 70% brightness
            draw.polygon([p0, p1, p2, p3], fill=fill_color, outline=(0, 0, 0), width=2)

    # 5. Draw 3D curved arrows
    if arrows:
        for face_name, direction, arrow_color in arrows:
            draw_curved_arrow(draw, face_name, direction, cx, cy, w, h, l, arrow_color, arrow_width=6)

    # Save to file
    img_path = os.path.join(SCRATCH_DIR, filename)
    img.save(img_path, "PNG")
    return img_path

def draw_curved_arrow(draw, face, direction, cx, cy, w, h, l, color=(236, 72, 153), arrow_width=6):
    """Draws a 3D-projected curved arrow on the specified face."""
    num_points = 30
    
    if face == 'U':
        # Up face: ellipse-like circle centered on Up face center
        rx = 1.15 * w * 1.5
        ry = 1.15 * h * 1.5
        # Clockwise goes right to left (0 -> 180 deg)
        start_angle = -20
        end_angle = 200
        if direction < 0:
            start_angle, end_angle = 200, -20
            
        angles = [start_angle + (end_angle - start_angle) * i / num_points for i in range(num_points + 1)]
        points = []
        for a in angles:
            rad = math.radians(a)
            x = cx + rx * math.cos(rad)
            y = cy + ry * math.sin(rad)
            points.append((x, y))
            
    elif face == 'R':
        # Right face: skewed circle
        rx_c = cx + 1.5 * w
        ry_c = cy + 1.5 * h + 1.5 * l
        r_scale = 1.05
        
        # Clockwise is increasing angle (150 -> 390 deg)
        start_angle = 150
        end_angle = 390
        if direction < 0:
            start_angle, end_angle = 390, 150
            
        angles = [start_angle + (end_angle - start_angle) * i / num_points for i in range(num_points + 1)]
        points = []
        for a in angles:
            rad = math.radians(a)
            u = r_scale * 1.5 * math.cos(rad)
            v = r_scale * 1.5 * math.sin(rad)
            x = rx_c + u * w
            y = ry_c + u * h + v * l
            points.append((x, y))
            
    elif face == 'F':
        # Front/Left face: skewed circle
        rx_c = cx - 1.5 * w
        ry_c = cy + 1.5 * h + 1.5 * l
        r_scale = 1.05
        
        # Clockwise is decreasing angle (30 -> -210 deg)
        start_angle = 30
        end_angle = -210
        if direction < 0:
            start_angle, end_angle = -210, 30
            
        angles = [start_angle + (end_angle - start_angle) * i / num_points for i in range(num_points + 1)]
        points = []
        for a in angles:
            rad = math.radians(a)
            u = r_scale * 1.5 * math.cos(rad)
            v = r_scale * 1.5 * math.sin(rad)
            x = rx_c - u * w
            y = ry_c + u * h + v * l
            points.append((x, y))
            
    else:
        return
        
    # Draw outline shadow/border for 3D effect
    outline_color = (0, 0, 0, 200)
    for i in range(len(points) - 1):
        draw.line([points[i], points[i+1]], fill=outline_color, width=arrow_width + 4)
    for i in range(len(points) - 1):
        draw.line([points[i], points[i+1]], fill=color, width=arrow_width)
        
    # Draw arrowhead
    p_last = points[-1]
    p_prev = points[-2]
    dx = p_last[0] - p_prev[0]
    dy = p_last[1] - p_prev[1]
    dist = math.hypot(dx, dy)
    if dist > 0:
        dx /= dist
        dy /= dist
        nx = -dy
        ny = dx
        arrow_size = 15
        p_tip = p_last
        p_base1 = (p_last[0] - dx * arrow_size + nx * arrow_size * 0.7, p_last[1] - dy * arrow_size + ny * arrow_size * 0.7)
        p_base2 = (p_last[0] - dx * arrow_size - nx * arrow_size * 0.7, p_last[1] - dy * arrow_size - ny * arrow_size * 0.7)
        # Draw outline arrowhead
        p_tip_o = (p_tip[0] + dx * 2, p_tip[1] + dy * 2)
        p_base1_o = (p_base1[0] - dx * 2 + nx * 2, p_base1[1] - dy * 2 + ny * 2)
        p_base2_o = (p_base2[0] - dx * 2 - nx * 2, p_base2[1] - dy * 2 - ny * 2)
        draw.polygon([p_tip_o, p_base1_o, p_base2_o], fill=outline_color)
        draw.polygon([p_tip, p_base1, p_base2], fill=color)

def generate_cube_images():
    print("Generating 3D Rubik's Cube images...")

    # Solved Cube Reference
    solved_u = [['Y']*3 for _ in range(3)]
    solved_l = [['B']*3 for _ in range(3)]
    solved_r = [['R']*3 for _ in range(3)]
    draw_isometric_cube(solved_u, solved_l, solved_r, filename="cube_solved.png")

    # Notation Examples (U and R moves)
    draw_isometric_cube(solved_u, solved_l, solved_r, [('U', 1, (236, 72, 153))], "cube_notation_u.png")
    draw_isometric_cube(solved_u, solved_l, solved_r, [('R', 1, (59, 130, 246))], "cube_notation_r.png")

    # Step 1: White Cross (White on top face for illustration)
    step1_u = [
        ['X', 'W', 'X'],
        ['W', 'W', 'W'],
        ['X', 'W', 'X']
    ]
    step1_l = [
        ['B', 'X', 'X'],
        ['B', 'B', 'X'],
        ['B', 'B', 'X']
    ]
    step1_r = [
        ['R', 'X', 'X'],
        ['R', 'R', 'X'],
        ['R', 'R', 'X']
    ]
    draw_isometric_cube(step1_u, step1_l, step1_r, filename="step1_white_cross.png")

    # Step 2: First Layer Corners (White face solved, top rows solved)
    step2_u = [['W']*3 for _ in range(3)]
    step2_l = [
        ['B', 'B', 'B'],
        ['B', 'B', 'X'],
        ['B', 'B', 'X']
    ]
    step2_r = [
        ['R', 'R', 'R'],
        ['R', 'R', 'X'],
        ['R', 'R', 'X']
    ]
    draw_isometric_cube(step2_u, step2_l, step2_r, [('R', 1, (239, 68, 68)), ('U', 1, (252, 211, 77))], filename="step2_white_face.png")

    # Step 3: Second Layer (Middle layer solved, White on bottom - not visible)
    # We turn it over so Yellow is U, Blue is L, Red is R.
    # Bottom layer (White) is solved. Middle layer edges solved. Top is unsolved (X/Yellow).
    step3_u = [['X', 'X', 'X'], ['X', 'Y', 'X'], ['X', 'X', 'X']]
    step3_l = [
        ['X', 'X', 'B'],
        ['X', 'B', 'B'],
        ['X', 'X', 'B']
    ]
    step3_r = [
        ['X', 'X', 'R'],
        ['X', 'R', 'R'],
        ['X', 'X', 'R']
    ]
    draw_isometric_cube(step3_u, step3_l, step3_r, [('U', 1, (16, 185, 129)), ('R', 1, (239, 68, 68))], filename="step3_middle_layer.png")

    # Step 4: Yellow Cross (Yellow edges oriented)
    step4_u = [
        ['X', 'Y', 'X'],
        ['Y', 'Y', 'Y'],
        ['X', 'Y', 'X']
    ]
    step4_l = [
        ['X', 'X', 'B'],
        ['B', 'B', 'B'],
        ['B', 'B', 'B']
    ]
    step4_r = [
        ['X', 'X', 'R'],
        ['R', 'R', 'R'],
        ['R', 'R', 'R']
    ]
    draw_isometric_cube(step4_u, step4_l, step4_r, [('F', 1, (249, 115, 22))], filename="step4_yellow_cross.png")

    # Step 5: Yellow Face (Sune OLL)
    step5_u = [['Y']*3 for _ in range(3)]
    step5_l = [
        ['X', 'X', 'B'],
        ['B', 'B', 'B'],
        ['B', 'B', 'B']
    ]
    step5_r = [
        ['X', 'X', 'R'],
        ['R', 'R', 'R'],
        ['R', 'R', 'R']
    ]
    draw_isometric_cube(step5_u, step5_l, step5_r, [('R', 1, (239, 68, 68)), ('U', 1, (252, 211, 77))], filename="step5_yellow_face.png")

    # Step 6: Permute Corners (A-Perm headlight)
    # Headlights on the Front-Left face: corners are Blue, middle is Green (unsolved).
    step6_u = [['Y']*3 for _ in range(3)]
    step6_l = [
        ['B', 'G', 'B'],
        ['B', 'B', 'B'],
        ['B', 'B', 'B']
    ]
    step6_r = [
        ['R', 'O', 'R'],
        ['R', 'R', 'R'],
        ['R', 'R', 'R']
    ]
    draw_isometric_cube(step6_u, step6_l, step6_r, [('R', -1, (239, 68, 68)), ('F', 1, (249, 115, 22))], filename="step6_yellow_corners.png")

    # Step 7: Permute Edges (Solved!)
    draw_isometric_cube(solved_u, solved_l, solved_r, [('R', 1, (59, 130, 246)), ('U', -1, (252, 211, 77))], filename="step7_yellow_edges.png")

    print("Cube images successfully generated!")

def build_pdf():
    pdf_path = os.path.join(OUTPUT_DIR, "Rubik's Cube Solution.pdf")
    print(f"Compiling PDF at {pdf_path}...")

    # Document Setup
    doc = SimpleDocTemplate(
        pdf_path,
        pagesize=letter,
        leftMargin=36,
        rightMargin=36,
        topMargin=36,
        bottomMargin=36
    )

    # Styles
    styles = getSampleStyleSheet()
    
    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Heading1'],
        fontName='Helvetica-Bold',
        fontSize=24,
        leading=28,
        textColor=colors.HexColor('#1E3A8A'),
        alignment=1, # Center
        spaceAfter=15
    )

    subtitle_style = ParagraphStyle(
        'DocSub',
        parent=styles['Normal'],
        fontName='Helvetica-Oblique',
        fontSize=12,
        leading=14,
        textColor=colors.HexColor('#4B5563'),
        alignment=1, # Center
        spaceAfter=25
    )

    heading_style = ParagraphStyle(
        'StepHeading',
        parent=styles['Heading2'],
        fontName='Helvetica-Bold',
        fontSize=15,
        leading=18,
        textColor=colors.HexColor('#1E3A8A'),
        spaceBefore=12,
        spaceAfter=6,
        keepWithNext=True
    )

    body_style = ParagraphStyle(
        'StepBody',
        parent=styles['BodyText'],
        fontName='Helvetica',
        fontSize=10,
        leading=14,
        textColor=colors.HexColor('#1F2937'),
        spaceAfter=10
    )

    algo_style = ParagraphStyle(
        'AlgoText',
        parent=styles['Normal'],
        fontName='Courier-Bold',
        fontSize=12,
        leading=14,
        textColor=colors.HexColor('#B91C1C'), # Deep Red
        spaceAfter=8
    )

    table_header_style = ParagraphStyle(
        'TableHeader',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=10,
        leading=12,
        textColor=colors.white
    )

    table_body_style = ParagraphStyle(
        'TableBody',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=11,
        textColor=colors.HexColor('#1F2937')
    )

    story = []

    # Title & Header
    story.append(Paragraph("Rubik's Cube Solver Solution Guide", title_style))
    story.append(Paragraph("A Step-by-Step Layer-by-Layer Beginner's Guide with 3D Visual Aids", subtitle_style))
    
    # Notation Table
    story.append(Paragraph("Understanding Face Rotation Notation", heading_style))
    story.append(Paragraph("Before you start, familiarize yourself with standard Rubik's cube notation. Each letter represents a 90-degree <b>clockwise</b> rotation of that face as you look at it. An apostrophe (<b>'</b>) indicates a <b>counter-clockwise</b> rotation.", body_style))

    # Notation grid table
    notation_data = [
        [Paragraph("Symbol", table_header_style), Paragraph("Meaning", table_header_style), Paragraph("Visualization & Arrow Action", table_header_style)],
        [
            Paragraph("<b>U</b> / <b>U'</b>", table_body_style),
            Paragraph("Up (Top Face) / Up Prime", table_body_style),
            RLImage(os.path.join(SCRATCH_DIR, "cube_notation_u.png"), width=70, height=70)
        ],
        [
            Paragraph("<b>R</b> / <b>R'</b>", table_body_style),
            Paragraph("Right Face / Right Prime", table_body_style),
            RLImage(os.path.join(SCRATCH_DIR, "cube_notation_r.png"), width=70, height=70)
        ],
        [
            Paragraph("<b>F</b> / <b>F'</b>", table_body_style),
            Paragraph("Front Face / Front Prime", table_body_style),
            Paragraph("Rotates the front face facing you. <b>F'</b> is counter-clockwise.", table_body_style)
        ],
        [
            Paragraph("<b>L</b> / <b>L'</b>", table_body_style),
            Paragraph("Left Face / Left Prime", table_body_style),
            Paragraph("Rotates the left face (opposite the Right face).", table_body_style)
        ]
    ]

    t = Table(notation_data, colWidths=[60, 160, 300])
    t.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor('#1E3A8A')),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('BOTTOMPADDING', (0,0), (-1,0), 6),
        ('TOPPADDING', (0,0), (-1,0), 6),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor('#D1D5DB')),
        ('BACKGROUND', (0,1), (-1,-1), colors.HexColor('#F9FAFB')),
    ]))
    story.append(t)
    story.append(Spacer(1, 15))
    story.append(PageBreak()) # Clean break to steps

    # Steps function
    def add_step(step_num, title, algo, desc, img_name):
        story.append(Paragraph(f"Step {step_num}: {title}", heading_style))
        
        # We will lay out the step with text on the left and the 3D cube illustration on the right
        img_path = os.path.join(SCRATCH_DIR, img_name)
        rl_img = RLImage(img_path, width=150, height=150)
        
        step_text = f"""
        <b>Goal:</b> {desc}<br/><br/>
        <b>Algorithm / Moves:</b>
        """
        
        text_cell = [
            Paragraph(step_text, body_style),
            Paragraph(algo if algo else "Intuitive (No algorithm needed)", algo_style)
        ]
        
        step_table = Table([[text_cell, rl_img]], colWidths=[360, 160])
        step_table.setStyle(TableStyle([
            ('VALIGN', (0,0), (-1,-1), 'TOP'),
            ('LEFTPADDING', (0,0), (-1,-1), 0),
            ('RIGHTPADDING', (0,0), (-1,-1), 0),
            ('BOTTOMPADDING', (0,0), (-1,-1), 10),
            ('TOPPADDING', (0,0), (-1,-1), 10),
            ('LINEBELOW', (0,0), (-1,-1), 0.5, colors.HexColor('#E5E7EB')),
        ]))
        story.append(step_table)
        story.append(Spacer(1, 10))

    # Add the 7 steps
    add_step(
        1, "The White Cross", "",
        "Create a White cross on the top face. Make sure the side facets of the White edge pieces match the color of the adjacent center pieces (e.g. White-Blue edge aligns with the Blue center).",
        "step1_white_cross.png"
    )

    add_step(
        2, "Solve White Corners (First Layer)", "R U R' U'",
        "Insert the four White corner pieces into their correct spots. Position a corner piece directly below its target spot, then repeat the algorithm <b>R U R' U'</b> until it is correctly placed and oriented.",
        "step2_white_face.png"
    )

    add_step(
        3, "Solve Middle Layer (F2L Edges)", "U R U' R' U' F' U F  /  U' L' U L U F U' F'",
        "Turn the cube over so Yellow is on Top. Locate a middle layer edge piece on the Top layer (that does not contain Yellow). Match its side color with the center, then perform the matching algorithm to insert it into the Left or Right slot.",
        "step3_middle_layer.png"
    )
    
    story.append(PageBreak()) # Move to next page for remaining steps

    add_step(
        4, "Orient Edges (Yellow Cross)", "F R U R' U' F'",
        "Create a Yellow cross on the Top face. Depending on the state (Dot, L-shape, or Bar), orient the cube as illustrated and perform the algorithm <b>F R U R' U' F'</b> to progress to the cross.",
        "step4_yellow_cross.png"
    )

    add_step(
        5, "Orient Corners (Yellow Top Face)", "R U R' U R U2 R'",
        "Orient all Yellow corners to make the entire Top face Yellow. If you have exactly one Yellow corner facing up (Sune state), put it in the bottom-left of the Top face and perform the algorithm.",
        "step5_yellow_face.png"
    )

    add_step(
        6, "Permute Corners (Headlights)", "R B' R F F R' B R F F R R",
        "Position the corners correctly. Look for two corners on the same side that have matching side colors ('headlights'). Put them in the Back, and run the algorithm. If no headlights exist, run it once to get them.",
        "step6_yellow_corners.png"
    )

    add_step(
        7, "Permute Edges (Complete Solve)", "R U' R U R U R U' R' U' R2",
        "Position the final four edge pieces to complete the Rubik's Cube. If one side is fully solved, put it in the Back and run the algorithm. Repeat until all edges are in their correct slots!",
        "step7_yellow_edges.png"
    )

    # Footer notice
    story.append(Spacer(1, 15))
    story.append(Paragraph("<font color='#4B5563'><i>Tip: If you get stuck at any step, use the <b>Auto Mode</b> solver on the application page to generate the exact sequence of moves from your current scrambled state.</i></font>", body_style))

    # Build document
    doc.build(story)
    print("PDF build complete!")

if __name__ == "__main__":
    generate_cube_images()
    build_pdf()
